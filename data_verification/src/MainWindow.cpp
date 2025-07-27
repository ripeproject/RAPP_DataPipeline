
#include "MainWindow.hpp"
#include "CeresDataVerifier.hpp"
#include "StringUtils.hpp"

#include <nlohmann/json.hpp>

#include <wx/thread.h>
#include <wx/config.h>

#include <cbdf/BlockDataFile.hpp>

#include <filesystem>
#include <atomic>
#include <memory>
#include <map>

using namespace std::filesystem;


namespace
{
	wxEvtHandler* g_pEventHandler = nullptr;

	void load_experiments(std::filesystem::directory_iterator dir_it, std::map<std::string, std::filesystem::directory_entry>& exp_files)
	{
		for (auto entry : dir_it)
		{
			if (entry.is_directory())
			{
				load_experiments(std::filesystem::directory_iterator(entry), exp_files);
			}

			else if (entry.is_regular_file())
			{
				try
				{
					std::ifstream in;
					in.open(entry.path().string());

					if (!in.is_open())
					{
						in.close();
						continue;
					}

					nlohmann::json jsonDoc = nlohmann::json::parse(in, nullptr, false, true);

					std::string measurement_name;
					if (jsonDoc.contains("measurement name"))
						measurement_name = jsonDoc["measurement name"];
					else if (jsonDoc.contains("measurement_name"))
						measurement_name = jsonDoc["measurement_name"];
					else if (jsonDoc.contains("experiment name"))
						measurement_name = jsonDoc["experiment name"];
					else if (jsonDoc.contains("experiment_name"))
						measurement_name = jsonDoc["experiment_name"];

					if (measurement_name.empty())
					{
						in.close();
						continue;
					}

					in.close();
					auto filename = nStringUtils::safeFilename(measurement_name);

					exp_files.insert(std::make_pair(filename, entry));
				}
				catch (const std::exception& e)
				{
				}
			}
		}

	}
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

void complete_file_progress(const int id, std::string suffix)
{
	if (g_pEventHandler)
	{
		auto event = new cFileProgressEvent(COMPLETE_FILE_PROGRESS);
		event->SetFileProcessID(id);

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

	std::unique_ptr<wxConfig> config = std::make_unique<wxConfig>("CeresDataChecker");

	config->Read("SourceDataDirectory", &mSourceDataDirectory);

	if (!config->Read("InvalidDataDirectory", &mInvalidDataDirectory))
	{
		if (!mSourceDataDirectory.empty())
		{

			std::filesystem::path source_dir{ mSourceDataDirectory.ToStdString() };
			auto invalid_dir = source_dir / "invalid_data";

			mInvalidDataDirectory = invalid_dir.string();
		}
	}

	config->Read("ExperimentDirectory", &mExperimentDirectory);

	CreateControls();
	CreateLayout();
}

cMainWindow::~cMainWindow()
{
	g_pEventHandler = nullptr;

	delete mpOriginalLog;
	mpOriginalLog = nullptr;

	std::unique_ptr<wxConfig> config = std::make_unique<wxConfig>("CeresDataChecker");

	config->Write("SourceDataDirectory", mSourceDataDirectory);
	config->Write("InvalidDataDirectory", mInvalidDataDirectory);
	config->Write("ExperimentDirectory", mExperimentDirectory);
}

void cMainWindow::CreateControls()
{
	mpSourceCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(500, -1), wxTE_READONLY);
	mpSourceCtrl->SetValue(mSourceDataDirectory);

	mpSourceDirButton = new wxButton(this, wxID_ANY, "Browse");
	mpSourceDirButton->Bind(wxEVT_BUTTON, &cMainWindow::OnSourceDirectory, this);

	mpInvalidDataCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(500, -1), wxTE_READONLY);
	mpInvalidDataCtrl->SetValue(mInvalidDataDirectory);

	mpInvalidDataDirButton = new wxButton(this, wxID_ANY, "Browse");
	mpInvalidDataDirButton->Bind(wxEVT_BUTTON, &cMainWindow::OnInvalidDirectory, this);
	mpInvalidDataDirButton->Enable(false);
	mpInvalidDataDirButton->Show(false);

	mpExperimentCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(500, -1), wxTE_READONLY);
	mpExperimentCtrl->SetValue(mExperimentDirectory);

	mpExperimentDirButton = new wxButton(this, wxID_ANY, "Browse");
	mpExperimentDirButton->Bind(wxEVT_BUTTON, &cMainWindow::OnExperimentDirectory, this);

	mpVerifyButton = new wxButton(this, wxID_ANY, "Verify");
	mpVerifyButton->Bind(wxEVT_BUTTON, &cMainWindow::OnVerify, this);

	if (mSourceDataDirectory.empty() || mInvalidDataDirectory.empty())
		mpVerifyButton->Disable();

	mpProgressCtrl = new cFileProgressCtrl(this, wxID_ANY, "", 0, "Result", 75);
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
	grid_sizer->Add(mpInvalidDataCtrl, 1, wxEXPAND);
	grid_sizer->Add(mpInvalidDataDirButton, 0, wxALIGN_CENTER_VERTICAL);

	grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Experiment Directory: "), 0, wxALIGN_CENTER_VERTICAL);
	grid_sizer->Add(mpExperimentCtrl, 1, wxEXPAND);
	grid_sizer->Add(mpExperimentDirButton, 0, wxALIGN_CENTER_VERTICAL);
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
	auto invalid_dir = source_dir / "invalid_data";

	mInvalidDataDirectory = invalid_dir.string();
	mpInvalidDataCtrl->SetValue(mInvalidDataDirectory);

	mpVerifyButton->Enable();
}

void cMainWindow::OnInvalidDirectory(wxCommandEvent& WXUNUSED(event))
{
	wxDirDialog dlg(NULL, "Choose directory", mInvalidDataDirectory,
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;     // the user changed their mind...

	mInvalidDataDirectory = dlg.GetPath().ToStdString();
	mpInvalidDataCtrl->SetValue(mInvalidDataDirectory);
}

void cMainWindow::OnExperimentDirectory(wxCommandEvent& WXUNUSED(event))
{
	wxDirDialog dlg(NULL, "Select directory containing the experiment configuration files", mExperimentDirectory,
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;     // the user changed their mind...

	mExperimentDirectory = dlg.GetPath().ToStdString();
	mpExperimentCtrl->SetValue(mExperimentDirectory);
}

void cMainWindow::OnVerify(wxCommandEvent& WXUNUSED(event))
{
	const std::filesystem::path source_dir = mSourceDataDirectory.ToStdString();

	std::vector<directory_entry> files_to_verify;
	for (auto const& dir_entry : std::filesystem::directory_iterator{ source_dir })
	{
		if (!dir_entry.is_regular_file())
			continue;

		if (dir_entry.path().extension() != ".ceres")
			continue;

		files_to_verify.push_back(dir_entry);
	}

	if (files_to_verify.empty())
	{
		// No files to repair!
		return;
	}

	// Load experiment files for filename lookup
	std::map<std::string, std::filesystem::directory_entry> exp_files;

	if (!mExperimentDirectory.empty())
	{
		const std::filesystem::path exp_dir = mExperimentDirectory.ToStdString();
		load_experiments(std::filesystem::directory_iterator{ exp_dir }, exp_files);
	}

	const std::filesystem::path invalid_dir = mInvalidDataDirectory.ToStdString();

	int numOfFiles = 0;
	for (auto& file : files_to_verify)
	{
		auto file_info = nStringUtils::removeMeasurementTimestamp(file.path().filename().string());

		std::filesystem::path exp_file;

		if (!exp_files.empty())
		{
			auto it = exp_files.find(file_info.filename);

			if (it != exp_files.end())
				exp_file = it->second;
			else
			{
				exp_file.clear();
			}
		}

		cCeresDataVerifier* fp = new cCeresDataVerifier(numOfFiles++, invalid_dir, exp_file);

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

	g_num_failed_data_files = 0;
	g_num_invalid_data_files = 0;

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
	msg += mSourceDataDirectory;
	wxLogMessage(msg);

	if (g_num_failed_data_files != 0)
	{
		wxString msg = "Detected ";
		uint32_t l = g_num_failed_data_files;
		msg += wxString::Format(wxT("%d"), l);
		msg += " invalid files.  Please run FileChecker and FileRepair on this directory!";
		wxLogMessage(msg);
	}

	return (wxThread::ExitCode) 0;
}

