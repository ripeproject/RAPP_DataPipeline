
#include "MainWindow.hpp"

#include "FileProcessor.hpp"
#include "StringUtils.hpp"
#include "PlotBoundaries.hpp"

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

	std::unique_ptr<wxConfig> config = std::make_unique<wxConfig>("PlotSplit");

	config->Read("Files/Source", &mSrcDirectory);
	config->Read("Files/Destination", &mDstDirectory);
	config->Read("Files/ConfigurationFile", &mConfigFileName);
}

cMainWindow::~cMainWindow()
{
	std::unique_ptr<wxConfig> config = std::make_unique<wxConfig>("PlotSplit");

	config->Write("Files/Source", mSrcDirectory);
	config->Write("Files/Destination", mDstDirectory);
	config->Write("Files/ConfigurationFile", mConfigFileName);
}

void cMainWindow::OnComplete(wxCommandEvent& WXUNUSED(event))
{
	mpSplitButton->Enable();
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

	mpSavePlyFiles = new wxCheckBox(this, wxID_ANY, "Save PLY Files");
	mpPlyUseBinaryFormat = new wxCheckBox(this, wxID_ANY, "Use Binary Format");

	mpSavePlotsInSingleFile = new wxCheckBox(this, wxID_ANY, "Save In Single File");
	mpEnableFrameIDs = new wxCheckBox(this, wxID_ANY, "Enable Saving Frame IDs (If Present)");
	mpEnablePixelInfo = new wxCheckBox(this, wxID_ANY, "Enable Saving Pixel Info (If Present)");

	mpSplitButton = new wxButton(this, wxID_ANY, "Split");
	mpSplitButton->Disable();
	mpSplitButton->Bind(wxEVT_BUTTON, &cMainWindow::OnPlotSplit, this);

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

	topsizer->AddSpacer(5);

	wxBoxSizer* op_sizer = new wxBoxSizer(wxHORIZONTAL);

	{
		wxStaticBoxSizer* options = new wxStaticBoxSizer(wxHORIZONTAL, this, "Save Options");
		options->AddSpacer(5);
		options->Add(mpSavePlotsInSingleFile, wxSizerFlags().Proportion(0).Expand());
		options->AddSpacer(5);
		options->Add(mpEnableFrameIDs, wxSizerFlags().Proportion(0).Expand());
		options->AddSpacer(5);
		options->Add(mpEnablePixelInfo, wxSizerFlags().Proportion(0).Expand());

		op_sizer->Add(options, wxSizerFlags().Proportion(0).Expand());
	}

	op_sizer->AddSpacer(10);

	{
		wxStaticBoxSizer* options = new wxStaticBoxSizer(wxHORIZONTAL, this, "Export Options");
		options->AddSpacer(5);
		options->Add(mpSavePlyFiles, wxSizerFlags().Proportion(0).Expand());
		options->AddSpacer(5);
		options->Add(mpPlyUseBinaryFormat, wxSizerFlags().Proportion(0).Expand());

		op_sizer->Add(options, wxSizerFlags().Proportion(0).Expand());
	}

	topsizer->Add(op_sizer, wxSizerFlags().Proportion(0).Expand());

	topsizer->AddSpacer(5);
	topsizer->Add(mpSplitButton, wxSizerFlags().Proportion(0).Expand());
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

//	if ((mConfigData && !mConfigData->empty()) && (!mDstDirectory.IsEmpty()))
	if (!mConfigData.empty() && !mDstDirectory.IsEmpty())
		mpSplitButton->Enable();
}

void cMainWindow::OnDstBrowse(wxCommandEvent& event)
{
	wxDirDialog dlg(NULL, "Choose Destination Directory", mDstDirectory,
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;     // the user changed their mind...

	mDstDirectory = dlg.GetPath().ToStdString();
	mpLoadDstDir->SetValue(mDstDirectory);

//	if ((mConfigData && !mConfigData->empty()) && (!mSrcDirectory.IsEmpty()))
	if (!mConfigData.empty() && !mSrcDirectory.IsEmpty())
		mpSplitButton->Enable();
}

void cMainWindow::OnCfgBrowse(wxCommandEvent& event)
{
	wxFileName fullname(mConfigFileName);
	auto path = fullname.GetPath();
	auto name = fullname.GetName();

	wxFileDialog dlg(this, _("Open file"), path, name,
		"Config files (*.json)|*.json", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;     // the user changed their mind...

	std::string config_file = dlg.GetPath().ToStdString();

//	mConfigData = std::make_unique<cConfigFileData>(config_file);

//	if (!mConfigData->load())
	if (!mConfigData.open(config_file))
	{
		mpLoadConfigFile->SetValue("");
		mpSplitButton->Disable();
		return;
	}

	mConfigFileName = dlg.GetPath();
	mpLoadConfigFile->SetValue(mConfigFileName);

/*
	mpSavePlotsInSingleFile->SetValue(mConfigData->savePlotsInSingleFile());

	mpEnableFrameIDs->SetValue(mConfigData->saveFrameIds());
	mpEnablePixelInfo->SetValue(mConfigData->savePixelInfo());

	mpSavePlyFiles->SetValue(mConfigData->savePlysFiles());
	mpPlyUseBinaryFormat->SetValue(mConfigData->plysUseBinaryFormat());
*/

	const auto& options = mConfigData.getOptions();

	mpSavePlotsInSingleFile->SetValue(options.getSavePlotsInSingleFile());

	mpEnableFrameIDs->SetValue(options.getSaveFrameIds());
	mpEnablePixelInfo->SetValue(options.getSavePixelInfo());

	mpSavePlyFiles->SetValue(options.getSavePlyFiles());
	mpPlyUseBinaryFormat->SetValue(options.getPlysUseBinaryFormat());

	if (!mDstDirectory.IsEmpty() && !mSrcDirectory.IsEmpty())
		mpSplitButton->Enable();
}

void cMainWindow::OnPlotSplit(wxCommandEvent& WXUNUSED(event))
{
	using namespace std::filesystem;
	using namespace nStringUtils;


	std::string input_directory = mSrcDirectory.ToStdString();
	std::string output_directory = mDstDirectory.ToStdString();

	const std::filesystem::path input{ input_directory };

	std::vector<directory_entry> files_to_process;

	std::string month_dir;

	/*
	 * Create list of files to process
	 */
	{
		if (input.has_parent_path())
		{
			month_dir = input.filename().string();
			if (!isMonthDirectory(month_dir))
				month_dir.clear();
		}

		// Scan input directory for all CERES files to operate on
		for (auto const& dir_entry : std::filesystem::directory_iterator{ input })
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

	bool savePlotsInSingleFile = mpSavePlotsInSingleFile->GetValue();
	bool savePlyFiles = mpSavePlyFiles->GetValue();
	bool plyUseBinaryFormat = mpPlyUseBinaryFormat->GetValue();
	bool enableFrameIds = mpEnableFrameIDs->GetValue();
	bool enablePixelInfo = mpEnablePixelInfo->GetValue();


	/*
	 * Add all of the files to process
	 */
	int numFilesToProcess = 0;
	for (auto& in_file : files_to_process)
	{
		auto fe = removeProcessedTimestamp(in_file.path().filename().string());

		std::filesystem::path out_file = output_dir;

		if (out_file.empty())
			out_file = mDstDirectory.ToStdString();

		out_file /= fe.filename;

		auto it = mConfigData.find_by_filename(in_file.path().filename().string());
		if (it == mConfigData.end())
			continue;

		cFileProcessor* fp = new cFileProcessor(numFilesToProcess++, in_file, out_file, *it);

		fp->savePlotsInSingleFile(savePlotsInSingleFile);
		fp->savePlyFiles(savePlyFiles);
		fp->plyUseBinaryFormat(plyUseBinaryFormat);
		fp->enableSavingFrameIDs(enableFrameIds);
		fp->enableSavingPixelInfo(enablePixelInfo);

//Fix		fp->setPlotInfo(mConfigData->getPlotInfo(in_file.path().filename().string()));

		mFileProcessors.push(fp);
	}

	mpSplitButton->Disable();

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

	wxString msg = "Finished processing ";
	msg += mSrcDirectory;
	wxLogMessage(msg);

	auto event = new wxCommandEvent(COMPUTATION_COMPLETE);

	wxQueueEvent(this, event);

	return (wxThread::ExitCode) 0;
}


