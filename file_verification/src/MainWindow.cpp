
#include "MainWindow.hpp"
#include "CeresFileVerifier.hpp"
#include "LidarFileVerifier.hpp"
#include "StringUtils.hpp"

#include <wx/thread.h>
#include <wx/config.h>

#include <cbdf/BlockDataFile.hpp>

#include <filesystem>
#include <memory>
#include <atomic>

using namespace std::filesystem;

namespace
{
	wxEvtHandler* g_pEventHandler = nullptr;
}

std::atomic<uint32_t> g_num_failed_files = 0;

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

	std::unique_ptr<wxConfig> config = std::make_unique<wxConfig>("CeresFileChecker");

	config->Read("SourceDataDirectory", &mSourceDataDirectory);

	if (!config->Read("FailedDataDirectory", &mFailedDataDirectory))
	{
		if (!mSourceDataDirectory.empty())
		{

			std::filesystem::path source_dir{ mSourceDataDirectory.ToStdString() };
			auto failed_dir = source_dir / "failed_files";

			mFailedDataDirectory = failed_dir.string();
		}
	}

	CreateControls();
	CreateLayout();
}

cMainWindow::~cMainWindow()
{
	g_pEventHandler = nullptr;

	delete mpOriginalLog;
	mpOriginalLog = nullptr;

	std::unique_ptr<wxConfig> config = std::make_unique<wxConfig>("CeresFileChecker");

	config->Write("SourceDataDirectory", mSourceDataDirectory);
	config->Write("FailedDataDirectory", mFailedDataDirectory);
}

void cMainWindow::CreateControls()
{
	mpSourceCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(500, -1), wxTE_READONLY);
	mpSourceCtrl->SetValue(mSourceDataDirectory);

	mpSourceDirButton = new wxButton(this, wxID_ANY, "Browse");
	mpSourceDirButton->Bind(wxEVT_BUTTON, &cMainWindow::OnSourceDirectory, this);

	mpFailedCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(500, -1), wxTE_READONLY);
	mpFailedCtrl->SetValue(mFailedDataDirectory);

	mpFailedDirButton = new wxButton(this, wxID_ANY, "Browse");
	mpFailedDirButton->Bind(wxEVT_BUTTON, &cMainWindow::OnFailedDirectory, this);
	mpFailedDirButton->Enable(false);
	mpFailedDirButton->Show(false);

	mpVerifyButton = new wxButton(this, wxID_ANY, "Verify");
	mpVerifyButton->Disable();
	mpVerifyButton->Bind(wxEVT_BUTTON, &cMainWindow::OnVerify, this);

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
	grid_sizer->Add(mpSourceCtrl, 1, wxEXPAND);
	grid_sizer->Add(mpSourceDirButton, 0, wxALIGN_CENTER_VERTICAL);

	grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Failed Directory: "), 0, wxALIGN_CENTER_VERTICAL);
	grid_sizer->Add(mpFailedCtrl, 1, wxEXPAND);
	grid_sizer->Add(mpFailedDirButton, 0, wxALIGN_CENTER_VERTICAL);
	topsizer->Add(grid_sizer, wxSizerFlags().Proportion(0).Expand());

	topsizer->AddSpacer(5);
	topsizer->Add(mpVerifyButton, wxSizerFlags().Proportion(0).Expand());
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
void cMainWindow::OnSourceDirectory(wxCommandEvent& WXUNUSED(event))
{
	wxDirDialog dlg(NULL, "Choose directory", mSourceDataDirectory,
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;     // the user changed their mind...

	mSourceDataDirectory = dlg.GetPath().ToStdString();
	mpSourceCtrl->SetValue(mSourceDataDirectory);

	std::filesystem::path source_dir{ mSourceDataDirectory.ToStdString() };
	auto failed_dir = source_dir / "failed_files";

	mFailedDataDirectory = failed_dir.string();
	mpFailedCtrl->SetValue(mFailedDataDirectory);

	mpVerifyButton->Enable();
}

void cMainWindow::OnFailedDirectory(wxCommandEvent& WXUNUSED(event))
{
	wxDirDialog dlg(NULL, "Choose directory", mFailedDataDirectory,
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;     // the user changed their mind...

	mFailedDataDirectory = dlg.GetPath().ToStdString();
	mpFailedCtrl->SetValue(mFailedDataDirectory);
}

void cMainWindow::OnVerify(wxCommandEvent& WXUNUSED(event))
{
	const std::filesystem::path source_dir = mSourceDataDirectory.ToStdString();

	std::vector<directory_entry> ceres_files_to_verify;
	std::vector<directory_entry> lidar_files_to_verify;
	for (auto const& dir_entry : std::filesystem::directory_iterator{ source_dir })
	{
		if (!dir_entry.is_regular_file())
			continue;

		if (dir_entry.path().extension() == ".ceres")
			ceres_files_to_verify.push_back(dir_entry);

		if (dir_entry.path().extension() == ".lidar_data")
			lidar_files_to_verify.push_back(dir_entry);
	}

	if (ceres_files_to_verify.empty() && lidar_files_to_verify.empty())
	{
		// No files to repair!
		return;
	}

	const std::filesystem::path failed_dir = mFailedDataDirectory.ToStdString();
	int numOfFiles = 0;
	for (auto& file : ceres_files_to_verify)
	{
		cCeresFileVerifier* fp = new cCeresFileVerifier(numOfFiles++, failed_dir);

		if (fp->setFileToCheck(file))
		{
			mFileProcessors.push(fp);
		}
		else
		{
			delete fp;
		}
	}

	for (auto& file : lidar_files_to_verify)
	{
		cLidarFileVerifier* fp = new cLidarFileVerifier(numOfFiles++, failed_dir);

		if (fp->setFileToCheck(file))
		{
			mFileProcessors.push(fp);
		}
		else
		{
			delete fp;
		}
	}

	wxString msg = "Processing ";
	msg += wxString::Format("%d", numOfFiles);
	msg += " files from ";
	msg += mSourceDataDirectory;
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
	g_num_failed_files = 0;

	while (mFileProcessors.size() > 0)
	{
		if (GetThread()->TestDestroy())
			break;

		auto* fp = mFileProcessors.front();
		fp->process_file();
		mFileProcessors.pop();
		delete fp;
	}

	if (g_num_failed_files == 0)
	{
		wxString msg = "Finished processing ";
		msg += mSourceDataDirectory;
		wxLogMessage(msg);
	}
	else
	{
		wxString msg = "Finished processing ";
		msg += mSourceDataDirectory;
		if (g_num_failed_files == 1)
			msg += ", 1 file failed!";
		else
		{
			msg += ", ";
			int i = g_num_failed_files;
			msg += wxString::Format(wxT("%d"), i);
			msg += " files failed!";
		}
		wxLogError(msg);
	}

	return (wxThread::ExitCode) 0;
}

