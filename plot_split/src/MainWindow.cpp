
#include "MainWindow.hpp"

#include "FileProcessor.hpp"
#include "StringUtils.hpp"
#include "PlotBoundaries.hpp"

#include <wx/aui/framemanager.h>
#include <wx/aui/auibook.h>
#include <wx/aboutdlg.h>
#include <wx/thread.h>

#include <cbdf/BlockDataFile.hpp>

#include <filesystem>
#include <memory>

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

void update_prefix_progress(const int id, std::string prefix, const int progress_pct)
{
	if (g_pEventHandler)
	{
		auto event = new cFileProgressEvent(UPDATE_FILE_PROGRESS);
		event->SetFileProcessID(id);
		event->SetPrefix(prefix);
		event->SetProgress_pct(progress_pct);

		wxQueueEvent(g_pEventHandler, event);
	}
}

void complete_file_progress(const int id)
{
	if (g_pEventHandler)
	{
		auto event = new cFileProgressEvent(COMPLETE_FILE_PROGRESS);
		event->SetFileProcessID(id);

		wxQueueEvent(g_pEventHandler, event);
	}
}

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
	// Custom submenu items
	ID_RECONNECT = wxID_HIGHEST + 1,

};

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
//	mpHandler = GetEventHandler();

	CreateControls();
	CreateLayout();
}

cMainWindow::~cMainWindow()
{
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

	mpSplitButton = new wxButton(this, wxID_ANY, "Split");
	mpSplitButton->Disable();
	mpSplitButton->Bind(wxEVT_BUTTON, &cMainWindow::OnPlotSplit, this);

	mpProgressCtrl = new cFileProgressCtrl(this, wxID_ANY);
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
	wxDirDialog dlg(NULL, "Choose source directory", "",
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	dlg.ShowModal();

	mSrcDirectory = dlg.GetPath().ToStdString();
	mpSrcDirTextCtrl->SetValue(mSrcDirectory);

	if ((mConfigData && !mConfigData->empty()) && (!mDstDirectory.IsEmpty()))
		mpSplitButton->Enable();
}

void cMainWindow::OnDstBrowse(wxCommandEvent& event)
{
	wxDirDialog dlg(NULL, "Choose Destination Directory", "",
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;     // the user changed their mind...

	mDstDirectory = dlg.GetPath().ToStdString();
	mpLoadDstDir->SetValue(mDstDirectory);

	if ((mConfigData && !mConfigData->empty()) && (!mSrcDirectory.IsEmpty()))
		mpSplitButton->Enable();
}

void cMainWindow::OnCfgBrowse(wxCommandEvent& event)
{
	wxFileDialog dlg(this, _("Open file"), "", "",
		"Config files (*.json)|*.json", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;     // the user changed their mind...

	std::string config_file = dlg.GetPath().ToStdString();
	mpLoadConfigFile->SetValue(config_file);

	mConfigData = std::make_unique<cConfigFileData>(config_file);

	if (!mConfigData->load())
	{
		return;
	}

	if (!mDstDirectory.IsEmpty() && !mSrcDirectory.IsEmpty())
		mpSplitButton->Enable();
}

void cMainWindow::OnPlotSplit(wxCommandEvent& WXUNUSED(event))
{
	using namespace nStringUtils;

	const std::filesystem::path input{ mSrcDirectory.ToStdString() };

	std::vector<directory_entry> files_to_process;

	/*
	 * Create list of files to process
	 */
	{
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
		std::filesystem::path out_file;
		auto fe = removeProcessedTimestamp(in_file.path().filename().string());

		if (out_file.empty())
		{
			std::string out_filename = fe.filename;
			out_file = mDstDirectory.ToStdString();
			out_file /= addProcessedTimestamp(out_filename);

			if (!fe.extension.empty())
			{
				out_file.replace_extension(fe.extension);
			}
		}

		if (!mConfigData->hasPlotInfo(in_file.path().filename().string()))
			continue;

		cFileProcessor* fp = new cFileProcessor(numFilesToProcess++, in_file, out_file);

		fp->setPlotInfo(mConfigData->getPlotInfo(in_file.path().filename().string()));

		mFileProcessors.push(fp);
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
	msg += mSrcDirectory;
	wxLogMessage(msg);

	return (wxThread::ExitCode)0;
}

/*
void cMainFrame::OnThreadUpdate(wxThreadEvent& evt)
{
	SetStatusText(evt.GetString());
}


wxThread::ExitCode cMainFrame::Entry()
{
	// VERY IMPORTANT: this function gets executed in the secondary thread context!
	// Do not call any GUI function inside this function; rather use wxQueueEvent():
	cBlockDataFileReader data_file;

	data_file.open(mFilename);

	if (!data_file.isOpen())
	{
		wxThreadEvent* event = new wxThreadEvent();

		wxString msg = "Could not open ";
		msg += mFilename;
		msg += " for processing!";
		event->SetString(msg);
		wxQueueEvent(mpHandler, event);

		return (wxThread::ExitCode) 1;
	}

	{
		wxThreadEvent* event = new wxThreadEvent();

		wxString msg = "Processing: ";
		msg += mFilename;
		event->SetString(msg);
		wxQueueEvent(mpHandler, event);
	}

	try
	{
		while (!data_file.eof())
		{
			if (data_file.fail() || GetThread()->TestDestroy())
			{
				data_file.close();
				return (wxThread::ExitCode)0;
			}

			data_file.processBlock();
		}
	}
	catch (const std::exception& e)
	{
		wxThreadEvent* event = new wxThreadEvent();

		wxString msg = "Unknown Exception: ";
		msg += e.what();

		event->SetString(msg);
		wxQueueEvent(mpHandler, event);

		return (wxThread::ExitCode) 2;
	}

	{
		wxThreadEvent* event = new wxThreadEvent();

		event->SetString("Processing complete.");
		wxQueueEvent(mpHandler, event);
	}

	return (wxThread::ExitCode) 0;
}
*/

