
#include "MainWindow.hpp"

#include "FileProcessor.hpp"
#include "StringUtils.hpp"


#include <wx/aui/framemanager.h>
#include <wx/aui/auibook.h>
#include <wx/aboutdlg.h>
#include <wx/thread.h>
#include <wx/msgdlg.h>
#include <wx/config.h>
#include <wx/filename.h>

#include <cbdf/BlockDataFile.hpp>

#include <filesystem>
#include <memory>
#include <map>


using namespace std::filesystem;


namespace
{
	wxEvtHandler* g_pEventHandler = nullptr;

	std::map<int, int> prev_progress;

	bool byMonthAndDay(const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b)
	{
		auto dir1 = a.path().filename().string();
		auto dir2 = b.path().filename().string();

		auto month1 = nStringUtils::toMonth(dir1);
		auto month2 = nStringUtils::toMonth(dir2);

		if (month1 < month2)
			return true;

		if (month1 == month2)
		{
			auto day1 = nStringUtils::toDay(dir1);
			auto day2 = nStringUtils::toDay(dir2);

			return day1 < day2;
		}

		return false;
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

		prev_progress[id] = -1;
	}
}

void update_progress(const int id, const int progress_pct)
{
	if (g_pEventHandler)
	{
		if (prev_progress[id] != progress_pct)
		{
			auto event = new cFileProgressEvent(UPDATE_FILE_PROGRESS);
			event->SetFileProcessID(id);
			event->SetProgress_pct(progress_pct);

			wxQueueEvent(g_pEventHandler, event);
			prev_progress[id] = progress_pct;
		}
	}
}

void update_prefix_progress(const int id, std::string prefix, const int progress_pct)
{
	if (g_pEventHandler)
	{
		auto event = new cFileProgressEvent(UPDATE_FILE_PROGRESS);
		event->SetFileProcessID(id);
		event->SetPrefix(prefix);
		event->SetProgress_pct(progress_pct);

		wxQueueEvent(g_pEventHandler, event);

		prev_progress[id] = progress_pct;
	}
}

void complete_file_progress(const int id)
{
	if (g_pEventHandler)
	{
		auto event = new cFileProgressEvent(COMPLETE_FILE_PROGRESS);
		event->SetFileProcessID(id);

		wxQueueEvent(g_pEventHandler, event);

		prev_progress.erase(id);
	}
}

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
	// Custom submenu items
	ID_COMPLETE = wxID_HIGHEST + 1,

};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------
wxDEFINE_EVENT(COMPUTATION_COMPLETE, wxCommandEvent);

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(cMainWindow, wxPanel)
	EVT_COMMAND(wxID_ANY, COMPUTATION_COMPLETE, cMainWindow::OnComplete)
wxEND_EVENT_TABLE()



//-----------------------------------------------------------------------------
cMainWindow::cMainWindow(wxWindow* parent)
	: wxPanel(parent, wxID_ANY)
{
//	mpHandler = GetEventHandler();

	CreateControls();
	CreateLayout();

	std::unique_ptr<wxConfig> config = std::make_unique<wxConfig>("PlotData");

	config->Read("Files/Source", &mSrcDirectory);
	config->Read("Files/Destination", &mDstDirectory);
	config->Read("Files/ConfigurationFile", &mConfigFileName);
}

cMainWindow::~cMainWindow()
{
	std::unique_ptr<wxConfig> config = std::make_unique<wxConfig>("PlotData");

	config->Write("Files/Source", mSrcDirectory);
	config->Write("Files/Destination", mDstDirectory);
	config->Write("Files/ConfigurationFile", mConfigFileName);
}

void cMainWindow::OnComplete(wxCommandEvent& WXUNUSED(event))
{
	mpComputeButton->Enable();
}

void cMainWindow::CreateControls()
{
	mpSrcDirTextCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(500, -1), 0, wxTextValidator(wxFILTER_ALPHANUMERIC));
	mpSrcDirTextCtrl->Bind(wxEVT_KILL_FOCUS, &cMainWindow::OnValidateSrc, this);

	mpLoadSrcButton = new wxButton(this, wxID_ANY, "Browse");
	mpLoadSrcButton->Bind(wxEVT_BUTTON, &cMainWindow::OnSrcBrowse, this);

	mpLoadDstDir = new wxTextCtrl(this, wxID_ANY);
	mpLoadDstButton = new wxButton(this, wxID_ANY, "Browse");
	mpLoadDstButton->Bind(wxEVT_BUTTON, &cMainWindow::OnDstBrowse, this);

	mpLoadConfigFile = new wxTextCtrl(this, wxID_ANY);
	mpLoadConfigButton = new wxButton(this, wxID_ANY, "Browse");
	mpLoadConfigButton->Bind(wxEVT_BUTTON, &cMainWindow::OnCfgBrowse, this);

	mpSaveDataAsRowMajor = new wxCheckBox(this, wxID_ANY, "Save Data In Row Major Format");

	mpComputeButton = new wxButton(this, wxID_ANY, "Compute");
	mpComputeButton->Disable();
	mpComputeButton->Bind(wxEVT_BUTTON, &cMainWindow::OnCompute, this);

	mpProgressCtrl = new cFileProgressCtrl(this, wxID_ANY, "Phase", 150);
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

	grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Source: "), 0, wxALIGN_CENTER_VERTICAL);
	grid_sizer->Add(mpSrcDirTextCtrl, 1, wxEXPAND);
	grid_sizer->Add(mpLoadSrcButton, 0, wxALIGN_CENTER_VERTICAL);

	grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Destination: "), 0, wxALIGN_CENTER_VERTICAL);
	grid_sizer->Add(mpLoadDstDir, 1, wxEXPAND);
	grid_sizer->Add(mpLoadDstButton, 0, wxALIGN_CENTER_VERTICAL);

	grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Config File: "), 0, wxALIGN_CENTER_VERTICAL);
	grid_sizer->Add(mpLoadConfigFile, 1, wxEXPAND);
	grid_sizer->Add(mpLoadConfigButton, 0, wxALIGN_CENTER_VERTICAL);
	topsizer->Add(grid_sizer, wxSizerFlags().Proportion(0).Expand());

	topsizer->AddSpacer(10);

	wxBoxSizer* op_sizer = new wxBoxSizer(wxHORIZONTAL);

	{
		wxStaticBoxSizer* options = new wxStaticBoxSizer(wxHORIZONTAL, this, "Save Options");
		options->AddSpacer(5);

		options->Add(mpSaveDataAsRowMajor, wxSizerFlags().Proportion(0).Expand());

		options->AddSpacer(5);
		op_sizer->Add(options, wxSizerFlags().Proportion(0).Expand());
	}

/*
	op_sizer->AddSpacer(10);

	{
		wxStaticBoxSizer* options = new wxStaticBoxSizer(wxHORIZONTAL, this, "Export Options");
		options->AddSpacer(5);
		options->Add(mpSavePlyFiles, wxSizerFlags().Proportion(0).Expand());
		options->AddSpacer(5);
		options->Add(mpPlyUseBinaryFormat, wxSizerFlags().Proportion(0).Expand());

		op_sizer->Add(options, wxSizerFlags().Proportion(0).Expand());
	}
*/

	topsizer->Add(op_sizer, wxSizerFlags().Proportion(0).Expand());

	topsizer->AddSpacer(10);
	topsizer->Add(mpComputeButton, wxSizerFlags().Proportion(0).Expand());
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

void cMainWindow::OnValidateSrc(wxFocusEvent& event)
{
	event.Skip();
}

void cMainWindow::OnSrcBrowse(wxCommandEvent& WXUNUSED(event))
{
	wxDirDialog dlg(NULL, "Choose source directory", mSrcDirectory,
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	dlg.ShowModal();

	mSrcDirectory = dlg.GetPath().ToStdString();
	mpSrcDirTextCtrl->SetValue(mSrcDirectory);

	if (!mConfigFileName.IsEmpty() && !mDstDirectory.IsEmpty())
		mpComputeButton->Enable();
}

void cMainWindow::OnDstBrowse(wxCommandEvent& event)
{
	wxDirDialog dlg(NULL, "Choose Destination Directory", mDstDirectory,
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;     // the user changed their mind...

	mDstDirectory = dlg.GetPath().ToStdString();
	mpLoadDstDir->SetValue(mDstDirectory);

	if (!mConfigFileName.IsEmpty() && !mSrcDirectory.IsEmpty())
		mpComputeButton->Enable();
}

void cMainWindow::OnCfgBrowse(wxCommandEvent& event)
{
	wxFileName fullname(mConfigFileName);
	auto path = fullname.GetPath();
	auto name = fullname.GetName();

	wxFileDialog dlg(this, _("Open file"), path, name,
		"Plot Data Config files (*.plotdata)|*.plotdata", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;     // the user changed their mind...

	std::string config_file = dlg.GetPath().ToStdString();

	try
	{
		if (!mConfigData.open(config_file))
		{
			mpLoadConfigFile->SetValue("");
			mpComputeButton->Disable();
			return;
		}

		mConfigFileName = dlg.GetPath();
		mpLoadConfigFile->SetValue(mConfigFileName);

		mpSaveDataAsRowMajor->SetValue(mConfigData.getOptions().getSaveDataRowMajor());

		if (!mDstDirectory.IsEmpty() && !mSrcDirectory.IsEmpty())
			mpComputeButton->Enable();
	}
	catch (const std::logic_error& e)
	{
		console_message(e.what());
	}
}

void cMainWindow::OnCompute(wxCommandEvent& WXUNUSED(event))
{
	using namespace std::filesystem;
	using namespace nStringUtils;


	std::string input_directory = mSrcDirectory.ToStdString();
	std::string output_directory = mDstDirectory.ToStdString();

	const std::filesystem::path input{ input_directory };

	std::vector<directory_entry> days_to_process;

	/*
	 * Create list of days to process
	 */
	{
		if (input.has_parent_path())
		{
			std::string month_dir = input.filename().string();
			if (!isMonthDirectory(month_dir))
			{
				// Scan input directory for all MonthDay directory to operate on
				for (const auto& dir_entry : std::filesystem::directory_iterator{ input })
				{
					if (!dir_entry.is_directory())
						continue;

					std::string month_dir = dir_entry.path().filename().string();
					if (isMonthDirectory(month_dir))
					{
						days_to_process.push_back(dir_entry);
					}
				}

				std::sort(days_to_process.begin(), days_to_process.end(), &byMonthAndDay);
			}
		}
	}

	std::vector<directory_entry> files_to_process;

	std::string month_dir;

	/*
	 * Create list of files to process
	 */
	if (days_to_process.empty())
	{
		if (input.has_parent_path())
		{
			month_dir = input.filename().string();
			if (!isMonthDirectory(month_dir))
				month_dir.clear();
		}

		// Scan input directory for all CERES files to operate on
		for (const auto & dir_entry : std::filesystem::directory_iterator{ input })
		{
			if (!dir_entry.is_regular_file())
				continue;

			auto ext = dir_entry.path().extension();
			if (ext != ".ceres")
				continue;

			if (!isCeresFile(dir_entry.path().string()))
				continue;

			files_to_process.push_back(dir_entry);
		}
	}
	else
	{
		for (const auto& day : days_to_process)
		{
			// Scan input directory for all CERES files to operate on
			for (const auto& dir_entry : std::filesystem::directory_iterator{ day })
			{
				if (!dir_entry.is_regular_file())
					continue;

				auto ext = dir_entry.path().extension();
				if (ext != ".ceres")
					continue;

				if (!isCeresFile(dir_entry.path().string()))
					continue;

				files_to_process.push_back(dir_entry);
			}
		}

	}

	/*
	 * Make sure the output directory exists
	 */
	std::filesystem::path output{ output_directory };

	std::filesystem::directory_entry output_dir;

	{
		if (!month_dir.empty())
		{
			std::string last_dir;
			if (output.has_parent_path())
			{
				last_dir = output.filename().string();
			}

			if (month_dir != last_dir)
			{
				output /= month_dir;
			}
		}

		output_dir = std::filesystem::directory_entry{ output };
	}

	if (files_to_process.empty())
	{
		// No files to process!
		return;
	}


	/*
	 * Add all of the files to process
	 */
	int numFilesToProcess = 0;
	for (auto& in_file : files_to_process)
	{
		cFileProcessor* fp = new cFileProcessor(numFilesToProcess++, in_file, mResults, mConfigData);

		mFileProcessors.push(fp);
	}

	mpComputeButton->Disable();

	if (numFilesToProcess > 1)
	{
		wxString msg = "Processing ";
		msg += wxString::Format("%d", numFilesToProcess);
		msg += " files from ";
		msg += mSrcDirectory;
		wxLogMessage(msg);
	}

	if (!output_dir.exists() && (numFilesToProcess > 0))
	{
		std::filesystem::create_directories(output_dir);
	}

	mResults.setRootFileName(mConfigData.getResultsRootFileName());
	mResults.saveRowMajor(mpSaveDataAsRowMajor->IsChecked());

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
	try
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
	}
	catch (const std::exception& e)
	{
		const char* msg = e.what();
		wxLogError(msg);
	}

	mResults.computeReplicateData();

	mResults.write_metadata_file(mDstDirectory.ToStdString());

	mResults.write_weather_file(mDstDirectory.ToStdString());

	mResults.write_plot_num_points_file(mDstDirectory.ToStdString());

	if (mResults.hasGroupInfo())
	{
		mResults.write_replicate_num_points_file(mDstDirectory.ToStdString());
	}

	mResults.write_plot_height_file(mDstDirectory.ToStdString());

	if (mResults.hasGroupInfo())
	{
		mResults.write_replicate_height_file(mDstDirectory.ToStdString());
	}

	mResults.write_plot_biomass_file(mDstDirectory.ToStdString());

	if (mResults.hasGroupInfo())
	{
		mResults.write_replicate_biomass_file(mDstDirectory.ToStdString());
	}

	mResults.write_plot_lai_file(mDstDirectory.ToStdString());

	if (mResults.hasGroupInfo())
	{
		mResults.write_replicate_lai_file(mDstDirectory.ToStdString());
	}

	mResults.clear();

	wxString msg = "Finished processing ";
	msg += mSrcDirectory;
	wxLogMessage(msg);

	auto event = new wxCommandEvent(COMPUTATION_COMPLETE);

	wxQueueEvent(this, event);

	return (wxThread::ExitCode) 0;
}


