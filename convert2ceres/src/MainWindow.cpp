
#include "MainWindow.hpp"
#include "LidarData2CeresConverter.hpp"

#include <wx/aui/framemanager.h>
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

void update_file_progress(const int id, const int progress_pct)
{
	if (g_pEventHandler)
	{
		auto event = new cFileProgressEvent(UPDATE_FILE_PROGRESS);
		event->SetFileProcessID(id);
		event->SetProgress_pct(progress_pct);

		wxQueueEvent(g_pEventHandler, event);
	}
}

void complete_file_progress(const int id)
{
	if (g_pEventHandler)
	{
		auto event = new cFileProgressEvent(COMPLETE_FILE_PROGRESS);
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
	ID_COMPLETE = wxID_HIGHEST + 1,

};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------
wxDEFINE_EVENT(COMPUTATION_COMPLETE, wxCommandEvent);

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

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

void cMainWindow::OnComplete(wxCommandEvent& WXUNUSED(event))
{
	mpConvertButton->Enable();
}

void cMainWindow::CreateControls()
{
	mpSourceCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(500, -1), 0, wxTextValidator(wxFILTER_ALPHANUMERIC));
	mpSourceCtrl->Bind(wxEVT_KILL_FOCUS, &cMainWindow::OnValidateSrc, this);

	mpSrcFileButton = new wxButton(this, wxID_ANY, "File");
	mpSrcFileButton->Bind(wxEVT_BUTTON, &cMainWindow::OnSrcFile, this);

	mpSrcDirButton = new wxButton(this, wxID_ANY, "Directory");
	mpSrcDirButton->Bind(wxEVT_BUTTON, &cMainWindow::OnSrcDirectory, this);

	mpDstCtrl = new wxTextCtrl(this, wxID_ANY);
	mpDstButton = new wxButton(this, wxID_ANY, "Browse");
	mpDstButton->Bind(wxEVT_BUTTON, &cMainWindow::OnDstDirectory, this);

	mpConvertButton = new wxButton(this, wxID_ANY, "Convert");
	mpConvertButton->Disable();
	mpConvertButton->Bind(wxEVT_BUTTON, &cMainWindow::OnConvert, this);

	mpProgressCtrl = new cFileProgressCtrl(this, wxID_ANY, "");
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
	grid_sizer->Add(mpSourceCtrl, 1, wxEXPAND);
	grid_sizer->Add(mpSrcFileButton, 0, wxALIGN_CENTER_VERTICAL);
	grid_sizer->Add(mpSrcDirButton, 0, wxALIGN_CENTER_VERTICAL);

	grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Destination: "), 0, wxALIGN_CENTER_VERTICAL);
	grid_sizer->Add(mpDstCtrl, 1, wxEXPAND);
	grid_sizer->Add(mpDstButton, 0, wxALIGN_CENTER_VERTICAL);
	topsizer->Add(grid_sizer, wxSizerFlags().Proportion(0).Expand());

	topsizer->AddSpacer(5);
	topsizer->Add(mpConvertButton, wxSizerFlags().Proportion(0).Expand());
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

void cMainWindow::OnSrcFile(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog dlg(this, _("Open file"), "", "",
			"Lidar files (*.lidar_data)|*.lidar_data", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;     // the user changed their mind...

	mSourceIsFile = true;
	mSource = dlg.GetPath().ToStdString();
	mpSourceCtrl->SetValue(mSource);

	path dst(mSource);
	dst.replace_extension("ceres");
	mpDstCtrl->SetValue(dst.string());

	mpConvertButton->Enable();
}

void cMainWindow::OnSrcDirectory(wxCommandEvent& WXUNUSED(event))
{
	wxDirDialog dlg(NULL, "Choose input directory", "",
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;     // the user changed their mind...

	mSourceIsFile = false;
	mSource = dlg.GetPath().ToStdString();
	mpSourceCtrl->SetValue(mSource);

	if (!mpDstCtrl->GetValue().IsEmpty())
		mpConvertButton->Enable();
}

void cMainWindow::OnDstDirectory(wxCommandEvent& WXUNUSED(event))
{
	wxDirDialog dlg(NULL, "Choose output directory", "",
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;     // the user changed their mind...

	auto dir = dlg.GetPath().ToStdString();

	if (mSourceIsFile)
	{
		path dst(dir);
		path src(mSource);
		dst /= src.filename();
		dst.replace_extension("ceres");
		mpDstCtrl->SetValue(dst.string());
	}
	else
	{
		mpDstCtrl->SetValue(dir);
	}

	if (!mSource.empty())
	{
		mpConvertButton->Enable();
	}
}

void cMainWindow::OnConvert(wxCommandEvent& WXUNUSED(event))
{
	int numFilesToProcess = 0;

	if (mSourceIsFile)
	{
		std::filesystem::directory_entry in_file(mSource);

		std::filesystem::path out_file = mpDstCtrl->GetValue().ToStdString();
		out_file.replace_extension("ceres");

		if (std::filesystem::exists(out_file))
			return;

		cFileProcessor* fp = new cLidarData2CeresConverter(numFilesToProcess++, in_file, out_file);
		mFileProcessors.push(fp);

	}
	else
	{
		const std::filesystem::path input(mSource);
		const std::filesystem::path output{ mpDstCtrl->GetValue().ToStdString() };

		// Scan input directory for all non-CERES files to operate on
		std::vector<directory_entry> lidar_data_files_to_process;
		for (auto const& dir_entry : std::filesystem::directory_iterator{ input })
		{
			if (!dir_entry.is_regular_file())
				continue;

			// Don't process any ceres files!
			if (dir_entry.path().extension() == ".ceres")
				continue;

			// Test for the lidar_data extension...
			if (dir_entry.path().extension() == ".lidar_data")
			{
				// If there is already a file by the same name in
				// the output directory with a ceres extension,
				// skip the file!
				auto test = output;
				test /= dir_entry.path().filename();
				test.replace_extension("ceres");
				if (std::filesystem::exists(test))
					continue;

				lidar_data_files_to_process.push_back(dir_entry);
			}
		}

		for (auto& in_file : lidar_data_files_to_process)
		{
			std::filesystem::path out_file = output;
			out_file /= in_file.path().filename();
			out_file.replace_extension("ceres");

			cFileProcessor* fp = new cLidarData2CeresConverter(numFilesToProcess++, in_file, out_file);
			mFileProcessors.push(fp);
		}

		lidar_data_files_to_process.clear();
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
		auto* fp = mFileProcessors.front();
		fp->run();
		mFileProcessors.pop();
		delete fp;
	}

	return (wxThread::ExitCode) 0;
}

