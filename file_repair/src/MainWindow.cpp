
#include "MainWindow.hpp"
#include "FileProcessor.hpp"
#include "StringUtils.hpp"

#include <wx/thread.h>

#include <cbdf/BlockDataFile.hpp>

#include <filesystem>

using namespace std::filesystem;

namespace
{
	wxEvtHandler* g_pEventHandler = nullptr;
}

void console_message(const std::string& msg)
{
	wxLogMessage(wxString(msg));
}

void new_file_progress(const int id, std::string filename)
{
	if (g_pEventHandler)
	{
		auto event = new cFileProgressEvent(NEW_FILE_PROGRESS);
		event->SetFileProcessID(id);
		event->SetFileName(filename);

		wxQueueEvent(g_pEventHandler, event);
	}
}

void update_prefix_progress(const int id, std::string prefix, const int progress_pct)
{
	if (g_pEventHandler)
	{
		auto event = new cFileProgressEvent(UPDATE_FILE_PROGRESS);
		event->SetFileProcessID(id);

		if (!prefix.empty())
			event->SetPrefix(prefix);

		event->SetProgress_pct(progress_pct);

		wxQueueEvent(g_pEventHandler, event);
	}
}

void update_progress(const int id, const int progress_pct)
{
	if (g_pEventHandler)
	{
		auto event = new cFileProgressEvent(UPDATE_FILE_PROGRESS);
		event->SetFileProcessID(id);
		event->SetProgress_pct(progress_pct);

		wxQueueEvent(g_pEventHandler, event);
	}
}

void complete_file_progress(const int id, std::string prefix, std::string suffix)
{
	if (g_pEventHandler)
	{
		auto event = new cFileProgressEvent(COMPLETE_FILE_PROGRESS);
		event->SetFileProcessID(id);

		if (!prefix.empty())
		{
			event->SetPrefix(prefix);
		}

		if (!suffix.empty())
		{
			event->SetResult(suffix);
		}

		wxQueueEvent(g_pEventHandler, event);
	}
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
	g_pEventHandler = nullptr;

	delete mpOriginalLog;
	mpOriginalLog = nullptr;
}

void cMainWindow::CreateControls()
{
	mpFailedCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(500, -1), wxTE_READONLY);

	mpFailedDirButton = new wxButton(this, wxID_ANY, "Browse");
	mpFailedDirButton->Bind(wxEVT_BUTTON, &cMainWindow::OnFailedDirectory, this);

	mpPartialRepairCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(500, -1), wxTE_READONLY);

	mpPartialRepairDirButton = new wxButton(this, wxID_ANY, "Browse");
	mpPartialRepairDirButton->Bind(wxEVT_BUTTON, &cMainWindow::OnPartialRepairDirectory, this);

	mpFullRepairCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(500, -1), wxTE_READONLY);

	mpFullRepairDirButton = new wxButton(this, wxID_ANY, "Browse");
	mpFullRepairDirButton->Bind(wxEVT_BUTTON, &cMainWindow::OnFullRepairDirectory, this);

	mpRepairButton = new wxButton(this, wxID_ANY, "Repair");
	mpRepairButton->Disable();
	mpRepairButton->Bind(wxEVT_BUTTON, &cMainWindow::OnRepair, this);

	mpProgressCtrl = new cFileProgressCtrl(this, wxID_ANY, "Phase", 75, "Result", 25);
	g_pEventHandler = mpProgressCtrl;

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

	grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Partial Repaired Directory: "), 0, wxALIGN_CENTER_VERTICAL);
	grid_sizer->Add(mpPartialRepairCtrl, 1, wxEXPAND);
	grid_sizer->Add(mpPartialRepairDirButton, 0, wxALIGN_CENTER_VERTICAL);

	grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Fully Repaired Directory: "), 0, wxALIGN_CENTER_VERTICAL);
	grid_sizer->Add(mpFullRepairCtrl, 1, wxEXPAND);
	grid_sizer->Add(mpFullRepairDirButton, 0, wxALIGN_CENTER_VERTICAL);

	topsizer->Add(grid_sizer, wxSizerFlags().Proportion(0).Expand());

	topsizer->AddSpacer(5);
	topsizer->Add(mpRepairButton, wxSizerFlags().Proportion(0).Expand());
	topsizer->AddSpacer(5);
	topsizer->Add(mpProgressCtrl, wxSizerFlags().Proportion(1).Expand());
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
	wxDirDialog dlg(NULL, "Choose directory", mFailedDataDirectory, wxDD_DEFAULT_STYLE);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;     // the user changed their mind...

	mFailedDataDirectory = dlg.GetPath().ToStdString();
	mpFailedCtrl->SetValue(mFailedDataDirectory);

	std::filesystem::path failed_dir{ mFailedDataDirectory.ToStdString() };
	auto base_dir = failed_dir.parent_path();
	std::filesystem::path partial_repaired_dir = base_dir / "partial_repaired_files";
	std::filesystem::path fully_repaired_dir = base_dir / "fully_repaired_files";

	mPartialRepairedDataDirectory = partial_repaired_dir.string();
	mpPartialRepairCtrl->SetValue(mPartialRepairedDataDirectory);

	mFullyRepairedDataDirectory = fully_repaired_dir.string();
	mpFullRepairCtrl->SetValue(mFullyRepairedDataDirectory);

	mpRepairButton->Enable();
}

void cMainWindow::OnPartialRepairDirectory(wxCommandEvent& WXUNUSED(event))
{
	wxDirDialog dlg(NULL, "Choose directory", mPartialRepairedDataDirectory, wxDD_DEFAULT_STYLE);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;     // the user changed their mind...

	mPartialRepairedDataDirectory = dlg.GetPath().ToStdString();
	mpPartialRepairCtrl->SetValue(mPartialRepairedDataDirectory);
}

void cMainWindow::OnFullRepairDirectory(wxCommandEvent& WXUNUSED(event))
{
	wxDirDialog dlg(NULL, "Choose directory", mFullyRepairedDataDirectory, wxDD_DEFAULT_STYLE);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;     // the user changed their mind...

	mFullyRepairedDataDirectory = dlg.GetPath().ToStdString();
	mpFullRepairCtrl->SetValue(mFullyRepairedDataDirectory);
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

	const std::filesystem::path temporary_dir = failed_dir / "tmp";
	if (!std::filesystem::exists(temporary_dir))
	{
		std::filesystem::create_directory(temporary_dir);
	}

	const std::filesystem::path partial_repaired_dir = mPartialRepairedDataDirectory.ToStdString();
	const std::filesystem::path fully_repaired_dir = mFullyRepairedDataDirectory.ToStdString();

	int numFilesToProcess = 0;
	for (auto& file : files_to_repair)
	{
		cFileProcessor* fp = new cFileProcessor(numFilesToProcess++, temporary_dir, partial_repaired_dir, fully_repaired_dir);

		if (fp->setFileToRepair(file))
		{
			mFileProcessors.push(fp);
		}
		else
		{
			delete fp;
		}
	}

	wxString msg = "Processing ";
	msg += wxString::Format("%d", numFilesToProcess);
	msg += " files from ";
	msg += mFailedDataDirectory;
	wxLogMessage(msg);

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

	wxString msg = "Finished processing ";
	msg += mFailedDataDirectory;
	wxLogMessage(msg);

	return (wxThread::ExitCode) 0;
}

