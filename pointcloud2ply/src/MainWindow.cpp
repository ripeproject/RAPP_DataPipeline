
#include "MainWindow.hpp"
#include "pointcloud2ply.hpp"
#include "FileProcessor.hpp"
#include "StringUtils.hpp"

#include <wx/thread.h>
#include <wx/config.h>

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
	mpHandler = GetEventHandler();

	CreateControls();
	CreateLayout();

	std::unique_ptr<wxConfig> config = std::make_unique<wxConfig>("PointCloud2Ply");

	config->Read("Files/Source", &mSource);
	config->Read("Files/Destination", &mDestinationDataDirectory);
}

cMainWindow::~cMainWindow()
{
	std::unique_ptr<wxConfig> config = std::make_unique<wxConfig>("PointCloud2Ply");

	config->Write("Files/Source", mSource);
	config->Write("Files/Destination", mDestinationDataDirectory);

	g_pEventHandler = nullptr;

	delete mpOriginalLog;
	mpOriginalLog = nullptr;
}

void cMainWindow::OnComplete(wxCommandEvent& WXUNUSED(event))
{
	mpExportButton->Enable();
}

void cMainWindow::CreateControls()
{
	mpSrcCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(500, -1), wxTE_READONLY);

	mpSrcFileButton = new wxButton(this, wxID_ANY, "File");
	mpSrcFileButton->Bind(wxEVT_BUTTON, &cMainWindow::OnSourceFile, this);

	mpSrcDirButton = new wxButton(this, wxID_ANY, "Directory");
	mpSrcDirButton->Bind(wxEVT_BUTTON, &cMainWindow::OnSourceDirectory, this);

	mpDstCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(500, -1), wxTE_READONLY);

	mpDstDirButton = new wxButton(this, wxID_ANY, "Browse");
	mpDstDirButton->Bind(wxEVT_BUTTON, &cMainWindow::OnDestinationDirectory, this);

	mpIndividualPlyFiles = new wxCheckBox(this, wxID_ANY, "Save As Individual Ply Files");
	mpSavePositionFile = new wxCheckBox(this, wxID_ANY, "Save Dolly Position Ply File");
	mpUseBinaryFormat = new wxCheckBox(this, wxID_ANY, "Save in Binary Format");
	mpResetOrigin = new wxCheckBox(this, wxID_ANY, "Reset Origin");

	mpExportButton = new wxButton(this, wxID_ANY, "Export");
	mpExportButton->Disable();
	mpExportButton->Bind(wxEVT_BUTTON, &cMainWindow::OnExport, this);

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

	auto* grid_sizer = new wxFlexGridSizer(4);
	grid_sizer->SetVGap(5);
	grid_sizer->SetHGap(5);
	grid_sizer->AddGrowableCol(1, 1);

	grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Source Directory: "), 0, wxALIGN_CENTER_VERTICAL);
	grid_sizer->Add(mpSrcCtrl, 1, wxEXPAND);
	grid_sizer->Add(mpSrcFileButton, 0, wxALIGN_CENTER_VERTICAL);
	grid_sizer->Add(mpSrcDirButton, 0, wxALIGN_CENTER_VERTICAL);

	grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Destination Directory: "), 0, wxALIGN_CENTER_VERTICAL);
	grid_sizer->Add(mpDstCtrl, 1, wxEXPAND);
	grid_sizer->Add(mpDstDirButton, 0, wxALIGN_CENTER_VERTICAL);
	topsizer->Add(grid_sizer, wxSizerFlags().Proportion(0).Expand());
	
	topsizer->AddSpacer(10);

	auto* op_sz = new wxStaticBoxSizer(wxHORIZONTAL, this, "Options");
	op_sz->AddSpacer(10);
	op_sz->Add(mpIndividualPlyFiles, wxSizerFlags().Proportion(0).Expand());
	op_sz->AddSpacer(10);
	op_sz->Add(mpSavePositionFile, wxSizerFlags().Proportion(0).Expand());
	op_sz->AddSpacer(10);
	op_sz->Add(mpUseBinaryFormat, wxSizerFlags().Proportion(0).Expand());
	op_sz->AddSpacer(10);
	op_sz->Add(mpResetOrigin, wxSizerFlags().Proportion(0).Expand());
	topsizer->Add(op_sz, wxSizerFlags().Proportion(0).Expand());

	topsizer->AddSpacer(10);

	topsizer->Add(mpExportButton, wxSizerFlags().Proportion(0).Expand());
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
void cMainWindow::OnSourceFile(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog dlg(this, _("Open file"), mSource, "",
		"Ceres files (*.ceres)|*.ceres", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;     // the user changed their mind...

	mSource = dlg.GetPath().ToStdString();
	mpSrcCtrl->SetValue(mSource);

	mIsFile = true;

	if (!mpDstCtrl->GetValue().IsEmpty())
		mpExportButton->Enable();
}

void cMainWindow::OnSourceDirectory(wxCommandEvent& WXUNUSED(event))
{
	wxDirDialog dlg(NULL, "Choose directory", mSource,
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;     // the user changed their mind...

	mSource = dlg.GetPath().ToStdString();
	mpSrcCtrl->SetValue(mSource);

	mIsFile = false;

	if (!mpDstCtrl->GetValue().IsEmpty())
		mpExportButton->Enable();
}

void cMainWindow::OnDestinationDirectory(wxCommandEvent& WXUNUSED(event))
{
	wxDirDialog dlg(NULL, "Choose Destination Directory", mDestinationDataDirectory,
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;     // the user changed their mind...

	mDestinationDataDirectory = dlg.GetPath().ToStdString();
	mpDstCtrl->SetValue(mDestinationDataDirectory);

	if (!mpSrcCtrl->GetValue().IsEmpty())
		mpExportButton->Enable();
}

void cMainWindow::OnExport(wxCommandEvent& WXUNUSED(event))
{
	using namespace nStringUtils;

	cPointCloud2Ply::mIndividualPlyFiles = mpIndividualPlyFiles->GetValue();
	cPointCloud2Ply::mSaveDollyPositions = mpSavePositionFile->GetValue();
	cPointCloud2Ply::mUseBinaryFormat    = mpUseBinaryFormat->GetValue();
	cPointCloud2Ply::mResetOrigin        = mpResetOrigin->GetValue();

	const std::filesystem::path input{ mSource.ToStdString() };
	std::string output_directory = mDestinationDataDirectory.ToStdString();

	std::vector<directory_entry> files_to_process;

	std::string month_dir;

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
		wxString msg = "Scanning directory: ";
		msg += mSource;
		wxLogMessage(msg);

		/*
		 * Create list of files to process
		 */
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

	if (files_to_process.empty())
	{
		// No files to export!
		return;
	}


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

			out_file = output_dir;

			out_file /= addProcessedTimestamp(out_filename);

			if (!fe.extension.empty())
			{
				out_file.replace_extension(fe.extension);
			}
		}

		cFileProcessor* fp = new cFileProcessor(numFilesToProcess++, in_file, out_file);

		mFileProcessors.push(fp);
	}

	mpExportButton->Disable();

	if (numFilesToProcess > 1)
	{
		wxString msg = "Processing ";
		msg += wxString::Format("%d", numFilesToProcess);
		msg += " files from ";
		msg += mSource;
		wxLogMessage(msg);
	}
	else if (numFilesToProcess == 1)
	{
		wxString msg = "Processing ";
		msg += wxString::Format("%d", numFilesToProcess);
		msg += " file from ";
		msg += mSource;
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

