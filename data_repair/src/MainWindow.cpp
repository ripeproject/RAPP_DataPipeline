
#include "MainWindow.hpp"
#include "FileProcessor.hpp"

#include <wx/thread.h>

#include <cbdf/BlockDataFile.hpp>

#include <filesystem>

using namespace std::filesystem;


void console_message(const std::string& msg)
{
	wxLogMessage(wxString(msg));
}

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(cMainWindow, wxPanel)
wxEND_EVENT_TABLE()



//-----------------------------------------------------------------------------
cMainWindow::cMainWindow(wxWindow* parent)
	: wxPanel(parent, wxID_ANY)
{
	mpHandler = GetEventHandler();

	CreateControls();
	CreateLayout();
}

cMainWindow::~cMainWindow()
{
}

void cMainWindow::CreateControls()
{
	mpFailedCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(500, -1), wxTE_READONLY);

	mpFailedDirButton = new wxButton(this, wxID_ANY, "Browse");
	mpFailedDirButton->Bind(wxEVT_BUTTON, &cMainWindow::OnFailedDirectory, this);

	mpRepairCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(500, -1), wxTE_READONLY);

	mpRepairButton = new wxButton(this, wxID_ANY, "Repair");
	mpRepairButton->Disable();
	mpRepairButton->Bind(wxEVT_BUTTON, &cMainWindow::OnRepair, this);

	// redirect logs from our event handlers to text control
	mpLogCtrl = new wxTextCtrl(this, wxID_ANY, wxString(), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
	mpLogCtrl->SetMinSize(wxSize(-1, 100));
	mpOriginalLog = wxLog::SetActiveTarget(new wxLogTextCtrl(mpLogCtrl));
}

void cMainWindow::CreateLayout()
{
	wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
	topsizer->AddSpacer(10);

	auto* grid_sizer = new wxFlexGridSizer(3);
	grid_sizer->SetVGap(5);
	grid_sizer->SetHGap(5);
	grid_sizer->AddGrowableCol(1, 1);

	grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Source Directory: "), 0, wxALIGN_CENTER_VERTICAL);
	grid_sizer->Add(mpFailedCtrl, 1, wxEXPAND);
	grid_sizer->Add(mpFailedDirButton, 0, wxALIGN_CENTER_VERTICAL);

	grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Repaired Directory: "), 0, wxALIGN_CENTER_VERTICAL);
	grid_sizer->Add(mpRepairCtrl, 1, wxEXPAND);
	topsizer->Add(grid_sizer, wxSizerFlags().Proportion(0).Expand());

	topsizer->AddSpacer(5);
	topsizer->Add(mpRepairButton, wxSizerFlags().Proportion(0).Expand());
	topsizer->AddSpacer(5);
	topsizer->Add(mpLogCtrl, wxSizerFlags().Proportion(1).Expand());

	topsizer->Layout();

	SetSizerAndFit(topsizer);

	auto size = GetBestSize();
	size.IncBy(10);
	SetMinSize(size);
}

// event handlers
void cMainWindow::OnFailedDirectory(wxCommandEvent& WXUNUSED(event))
{
	wxDirDialog dlg(NULL, "Choose directory", "",
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;     // the user changed their mind...

	mFailedDataDirectory = dlg.GetPath().ToStdString();
	mpFailedCtrl->SetValue(mFailedDataDirectory);

	std::filesystem::path failed_dir{ mFailedDataDirectory.ToStdString() };
	auto base_dir = failed_dir.parent_path();
	auto repair_dir = base_dir / "repaired";

	mRepairedDataDirectory = repair_dir.string();
	mpRepairCtrl->SetValue(mRepairedDataDirectory);

	mpLogCtrl->Clear();

	mpRepairButton->Enable();
}

void cMainWindow::OnRepair(wxCommandEvent& WXUNUSED(event))
{
	const std::filesystem::path failed_dir = mFailedDataDirectory.ToStdString();

	std::vector<directory_entry> files_to_repair;
	for (auto const& dir_entry : std::filesystem::directory_iterator{ failed_dir })
	{
		if (!dir_entry.is_regular_file())
			continue;

		if (dir_entry.path().extension() != ".ceres")
			continue;

		files_to_repair.push_back(dir_entry);
	}

	if (files_to_repair.empty())
	{
		wxLogMessage(wxString("No files to repair!"));
		return;
	}

	const std::filesystem::path recovered_dir = failed_dir / "recovered";
	if (!std::filesystem::exists(recovered_dir))
	{
		std::filesystem::create_directory(recovered_dir);
	}

	const std::filesystem::path repaired_dir = mRepairedDataDirectory.ToStdString();
	if (!std::filesystem::exists(repaired_dir))
	{
		std::filesystem::create_directory(repaired_dir);
	}

	for (auto& file : files_to_repair)
	{
		cFileProcessor* fp = new cFileProcessor(recovered_dir, repaired_dir);

		if (fp->setFileToRepair(file))
		{
			mFileProcessors.push(fp);
		}
		else
		{
			delete fp;
		}
	}

	startDataProcessing();
}

void cMainWindow::startDataProcessing()
{
	auto* pThread = GetThread();
	if (pThread && pThread->IsRunning())
	{
		return;
	}

	// Now we can start processing the new data file
	if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
	{
		wxLogError("Could not create the worker thread!");
		return;
	}

	GetThread()->Run();
}


void cMainWindow::stopDataProcessing()
{
	// Stop the processing of any previous file
	auto* pThread = GetThread();
	if (pThread && pThread->IsRunning())
	{
//		wxString msg = "Stopping the processing of: ";
//		msg += mFilename;

//		SetStatusText(msg);

		pThread->Delete();
	}
}

wxThread::ExitCode cMainWindow::Entry()
{
	while (mFileProcessors.size() > 0)
	{
		if (GetThread()->TestDestroy())
			break;

		auto* fp = mFileProcessors.front();
		fp->process_file();
		mFileProcessors.pop();
		delete fp;
	}

	wxLogMessage(wxString("Repair Finished."));

	return (wxThread::ExitCode) 0;
}

