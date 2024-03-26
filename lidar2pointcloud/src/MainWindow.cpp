
#include "MainWindow.hpp"
#include "lidar2pointcloud.hpp"
#include "FileProcessor.hpp"
#include "StringUtils.hpp"
#include "GroundModelUtils.hpp"

#include "ConfigFileData.hpp"

#include <wx/thread.h>
#include <wx/msgdlg.h>
#include <wx/config.h>
#include <wx/filename.h>

#include <cbdf/BlockDataFile.hpp>

#include <memory>
#include <map>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif

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

void complete_file_progress(const int id)
{
	if (g_pEventHandler)
	{
		auto event = new cFileProgressEvent(COMPLETE_FILE_PROGRESS);
		event->SetProgress_pct(100);
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
	:
	wxPanel(parent, wxID_ANY)
{
	CreateControls();
	CreateLayout();

	std::unique_ptr<wxConfig> config = std::make_unique<wxConfig>("LidarConvert");

	config->Read("Files/Source", &mSource);
	config->Read("Files/Destination", &mDestination);
	config->Read("Files/ConfigurationFile", &mCfgFilename);
}

cMainWindow::~cMainWindow()
{
	std::unique_ptr<wxConfig> config = std::make_unique<wxConfig>("LidarConvert");

	config->Write("Files/Source", mSource);
	config->Write("Files/Destination", mDestination);
	config->Write("Files/ConfigurationFile", mCfgFilename);

	g_pEventHandler = nullptr;

	delete mpOriginalLog;
	mpOriginalLog = nullptr;
}

void cMainWindow::OnComplete(wxCommandEvent& WXUNUSED(event))
{
	mpComputeButton->Enable();
}

void cMainWindow::CreateControls()
{
	mpLoadSrcFile = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(500, -1), wxTE_READONLY);

	mpSrcFileButton = new wxButton(this, wxID_ANY, "File");
	mpSrcFileButton->Bind(wxEVT_BUTTON, &cMainWindow::OnSrcFileBrowse, this);

	mpSrcDirButton = new wxButton(this, wxID_ANY, "Directory");
	mpSrcDirButton->Bind(wxEVT_BUTTON, &cMainWindow::OnSrcDirBrowse, this);

	mpLoadDstFile = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(500, -1), wxTE_READONLY);

	mpLoadDstButton = new wxButton(this, wxID_ANY, "Browse");
	mpLoadDstButton->Bind(wxEVT_BUTTON, &cMainWindow::OnDstBrowse, this);

	mpLoadCfgFile = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(500, -1), wxTE_READONLY);
	mpLoadCfgButton = new wxButton(this, wxID_ANY, "Browse");
	mpLoadCfgButton->Bind(wxEVT_BUTTON, &cMainWindow::OnCfgFileBrowse, this);

	mpSavePlyFiles = new wxCheckBox(this, wxID_ANY, "Save PLY Files");
	mpPlyUseBinaryFormat = new wxCheckBox(this, wxID_ANY, "Use Binary Format");

	mpSaveCompactFile = new wxCheckBox(this, wxID_ANY, "Save Compact Point Cloud");
	mpSaveLidarFrameIds = new wxCheckBox(this, wxID_ANY, "Save LiDAR Frame IDs");
	mpSaveLidarPixelInfo = new wxCheckBox(this, wxID_ANY, "Save LiDAR Pixel Information");

	mpComputeButton = new wxButton(this, wxID_ANY, "Compute Point Cloud");
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

	auto* grid_sizer = new wxFlexGridSizer(4);
	grid_sizer->SetVGap(5);
	grid_sizer->SetHGap(5);
	grid_sizer->AddGrowableCol(1, 1);

	grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Source: "), 0, wxALIGN_CENTER_VERTICAL);
	grid_sizer->Add(mpLoadSrcFile, 1, wxEXPAND);
	grid_sizer->Add(mpSrcFileButton, 0, wxALIGN_CENTER_VERTICAL);
	grid_sizer->Add(mpSrcDirButton, 0, wxALIGN_CENTER_VERTICAL);

	grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Destination: "), 0, wxALIGN_CENTER_VERTICAL);
	grid_sizer->Add(mpLoadDstFile, 1, wxEXPAND);
	grid_sizer->Add(mpLoadDstButton, 0, wxALIGN_CENTER_VERTICAL);
	grid_sizer->AddStretchSpacer();

	grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Config File: "), 0, wxALIGN_CENTER_VERTICAL);
	grid_sizer->Add(mpLoadCfgFile, 1, wxEXPAND);
	grid_sizer->Add(mpLoadCfgButton, 0, wxALIGN_CENTER_VERTICAL);

	topsizer->Add(grid_sizer, wxSizerFlags().Proportion(0).Expand());

	topsizer->AddSpacer(5);

	wxBoxSizer* op_sizer = new wxBoxSizer(wxHORIZONTAL);

	{
		wxStaticBoxSizer* options = new wxStaticBoxSizer(wxHORIZONTAL, this, "Save Options");
		options->AddSpacer(5);
		options->Add(mpSaveCompactFile, wxSizerFlags().Proportion(0).Expand());
		options->AddSpacer(5);
		options->Add(mpSaveLidarFrameIds, wxSizerFlags().Proportion(0).Expand());
		options->AddSpacer(5);
		options->Add(mpSaveLidarPixelInfo, wxSizerFlags().Proportion(0).Expand());

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

	topsizer->AddSpacer(10);
	topsizer->Add(mpComputeButton, wxSizerFlags().Proportion(0).Expand());
	topsizer->AddSpacer(5);
	topsizer->Add(mpProgressCtrl, wxSizerFlags().Proportion(1).Expand());
	topsizer->AddSpacer(5);
	topsizer->Add(mpLogCtrl, wxSizerFlags().Proportion(1).Expand());

	topsizer->Layout();

	SetSizerAndFit(topsizer);
}

// event handlers

void cMainWindow::OnSrcFileBrowse(wxCommandEvent& WXUNUSED(event))
{
	std::filesystem::path fname = mSource.ToStdString();
	auto fpath = fname.parent_path();

	wxFileDialog dlg(this, _("Open file"), wxString(fpath.string()), wxString(fname.filename().string()),
		"Ceres files (*.ceres)|*.ceres", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;     // the user changed their mind...

	mIsFile = true;
	mSource = dlg.GetPath();
	mpLoadSrcFile->SetValue(mSource);

	if ((mConfigData && !mConfigData->empty()) && (!mDestination.IsEmpty()))
		mpComputeButton->Enable();
}

void cMainWindow::OnSrcDirBrowse(wxCommandEvent& WXUNUSED(event))
{
	wxDirDialog dlg(NULL, "Choose input directory", mSource,
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;

	mIsFile = false;
	mSource = dlg.GetPath();
	mpLoadSrcFile->SetValue(mSource);

	if ((mConfigData && !mConfigData->empty()) && (!mDestination.IsEmpty()))
		mpComputeButton->Enable();
}

void cMainWindow::OnDstBrowse(wxCommandEvent& event)
{
	wxDirDialog dlg(NULL, "Choose output directory", mDestination,
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;

	mDestination = dlg.GetPath();
	mpLoadDstFile->SetValue(mDestination);

	if ((mConfigData && !mConfigData->empty()) && (!mSource.IsEmpty()))
		mpComputeButton->Enable();
}

void cMainWindow::OnCfgFileBrowse(wxCommandEvent& event)
{
	wxFileName fullname(mCfgFilename);
	auto path = fullname.GetPath();
	auto name = fullname.GetName();

	wxFileDialog dlg(this, _("Configuration file"), path, name,
		"Config files (*.json)|*.json", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;     // the user changed their mind...

	std::string config_file = dlg.GetPath().ToStdString();

	mConfigData = std::make_unique<cConfigFileData>(config_file);

	if (!mConfigData->load())
	{
		mpLoadCfgFile->SetValue("");
		mpComputeButton->Disable();
		return;
	}

	mpLoadCfgFile->SetValue(config_file);

	mCfgFilename = dlg.GetPath();

	mpSaveCompactFile->SetValue(mConfigData->saveCompactPointCloud());
	mpSaveLidarFrameIds->SetValue(mConfigData->saveFrameIds());
	mpSaveLidarPixelInfo->SetValue(mConfigData->savePixelInfo());

	mpSavePlyFiles->SetValue(mConfigData->savePlyFiles());
	mpPlyUseBinaryFormat->SetValue(mConfigData->plyUseBinaryFormat());

	if (!mDestination.IsEmpty() && !mSource.IsEmpty())
		mpComputeButton->Enable();
}

void cMainWindow::OnCompute(wxCommandEvent& WXUNUSED(event))
{
	using namespace std::filesystem;
	using namespace nStringUtils;

	if (!is_ground_data_loaded())
	{
		wxMessageBox(wxT("Please use the file menu to load ground mesh data file."), wxT("Missing Ground Data"), wxICON_ERROR);
		return;
	}

	std::string input_directory = mSource.ToStdString();
	std::string output_directory = mDestination.ToStdString();

	const std::filesystem::path input{ input_directory };

	std::vector<directory_entry> files_to_process;

	/*
	 * Create list of files to process
	 */
	if (mIsFile)
	{
		auto dir_entry = std::filesystem::directory_entry{ input };

		if (!dir_entry.is_regular_file())
			return;

		if (dir_entry.path().extension() != ".ceres")
			return;

		if (!isCeresFile(dir_entry.path().string()))
			return;

		files_to_process.push_back(dir_entry);
	}
	else
	{
		// Scan input directory for all CERES files to operate on
		for (auto const& dir_entry : std::filesystem::directory_iterator{ input })
		{
			if (!dir_entry.is_regular_file())
				continue;

			if (dir_entry.path().extension() != ".ceres")
				continue;

			if (!isCeresFile(dir_entry.path().string()))
				continue;

			files_to_process.push_back(dir_entry);
		}
	}

	/*
	 * Make sure the output directory exists
	 */
	const std::filesystem::path output{ output_directory };

	std::filesystem::directory_entry output_dir;

	if (mIsFile && output.has_extension())
	{
		output_dir = std::filesystem::directory_entry{ output.parent_path() };
	}
	else
	{
		output_dir = std::filesystem::directory_entry{ output };
	}

	if (!output_dir.exists())
	{
		std::filesystem::create_directories(output_dir);
	}

	bool saveCompactFile = mpSaveCompactFile->GetValue();
	bool saveFrameIds  = mpSaveLidarFrameIds->GetValue();
	bool savePixelInfo = mpSaveLidarPixelInfo->GetValue();

	bool savePlyFiles = mpSavePlyFiles->GetValue();
	bool plyUseBinaryFormat = mpPlyUseBinaryFormat->GetValue();


	int numFilesToProcess = 0;

	/*
	 * Add all of the files to process to the thread pool
	 */
	for (auto& in_file : files_to_process)
	{
		std::filesystem::path out_file;
		auto fe = removeProcessedTimestamp(in_file.path().filename().string());

		if (out_file.empty())
		{
			std::string out_filename = fe.filename;
			out_file = output_directory;
			out_file /= addProcessedTimestamp(out_filename);

			if (!fe.extension.empty())
			{
				out_file += ".";
				out_file += fe.extension;
			}
		}

		auto options = mConfigData->getParameters(in_file.path().filename().string());

		if (!options.has_value())
			continue;

		nConfigFileData::sParameters_t parameters = options.value();

		cFileProcessor* fp = new cFileProcessor(numFilesToProcess, in_file, out_file);

		++numFilesToProcess;

		fp->saveCompactPointCloud(saveCompactFile);
		fp->saveFrameIds(saveFrameIds);
		fp->savePixelInfo(savePixelInfo);

		fp->savePlyFiles(savePlyFiles);
		fp->plyUseBinaryFormat(plyUseBinaryFormat);
		fp->setDefaults(parameters);

		mFileProcessors.push(fp);
	}

	mpComputeButton->Disable();

	if (numFilesToProcess > 1)
	{
		wxString msg = "Processing ";
		msg += wxString::Format("%d", numFilesToProcess);
		msg += " files from ";
		msg += mSource;
		wxLogMessage(msg);
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

	wxString msg = "Finished processing ";
	msg += mSource;
	wxLogMessage(msg);

	auto event = new wxCommandEvent(COMPUTATION_COMPLETE);

	wxQueueEvent(this, event);

	return (wxThread::ExitCode) 0;
}

