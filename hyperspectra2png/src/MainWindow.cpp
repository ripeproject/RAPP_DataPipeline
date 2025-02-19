
#include "MainWindow.hpp"
#include "HySpexVNIR3000N_2_PNG.hpp"
#include "HySpexSWIR384_2_PNG.hpp"
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
	mpSrcCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(500, -1), wxTE_READONLY);

	mpSrcFileButton = new wxButton(this, wxID_ANY, "File");
	mpSrcFileButton->Bind(wxEVT_BUTTON, &cMainWindow::OnSourceFile, this);

	mpSrcDirButton = new wxButton(this, wxID_ANY, "Directory");
	mpSrcDirButton->Bind(wxEVT_BUTTON, &cMainWindow::OnSourceDirectory, this);

	mpDstCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(500, -1), wxTE_READONLY);

	mpDstDirButton = new wxButton(this, wxID_ANY, "Browse");
	mpDstDirButton->Bind(wxEVT_BUTTON, &cMainWindow::OnDestinationDirectory, this);

	mpVnirRedCtrl   = new wxTextCtrl(this, wxID_ANY);
	mpVnirGreenCtrl = new wxTextCtrl(this, wxID_ANY);
	mpVnirBlueCtrl  = new wxTextCtrl(this, wxID_ANY);

	mpVnirRedCtrl->SetValue(wxString::Format(wxT("%4.2f"), mVnirRed_nm) );
	mpVnirGreenCtrl->SetValue(wxString::Format(wxT("%4.2f"), mVnirGreen_nm));
	mpVnirBlueCtrl->SetValue(wxString::Format(wxT("%4.2f"), mVnirBlue_nm));

	mpSwirRedCtrl   = new wxTextCtrl(this, wxID_ANY);
	mpSwirGreenCtrl = new wxTextCtrl(this, wxID_ANY);
	mpSwirBlueCtrl  = new wxTextCtrl(this, wxID_ANY);

	mpSwirRedCtrl->SetValue(wxString::Format(wxT("%5.2f"), mSwirRed_nm));
	mpSwirGreenCtrl->SetValue(wxString::Format(wxT("%5.2f"), mSwirGreen_nm));
	mpSwirBlueCtrl->SetValue(wxString::Format(wxT("%5.2f"), mSwirBlue_nm));

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

	wxBoxSizer* options_sizer = new wxBoxSizer(wxHORIZONTAL);

	auto* vnir_sz = new wxStaticBoxSizer(wxHORIZONTAL, this, "VNIR");
	vnir_sz->Add(new wxStaticText(this, wxID_ANY, "Red (nm) : "), 0, wxALIGN_CENTER_VERTICAL);
	vnir_sz->AddSpacer(10);
	vnir_sz->Add(mpVnirRedCtrl, wxSizerFlags().Proportion(0).Expand());
	vnir_sz->AddSpacer(10);
	vnir_sz->Add(new wxStaticText(this, wxID_ANY, "Green (nm) : "), 0, wxALIGN_CENTER_VERTICAL);
	vnir_sz->AddSpacer(10);
	vnir_sz->Add(mpVnirGreenCtrl, wxSizerFlags().Proportion(0).Expand());
	vnir_sz->AddSpacer(10);
	vnir_sz->Add(new wxStaticText(this, wxID_ANY, "Blue (nm) : "), 0, wxALIGN_CENTER_VERTICAL);
	vnir_sz->AddSpacer(10);
	vnir_sz->Add(mpVnirBlueCtrl, wxSizerFlags().Proportion(0).Expand());

	auto* swir_sz = new wxStaticBoxSizer(wxHORIZONTAL, this, "SWIR");
	swir_sz->Add(new wxStaticText(this, wxID_ANY, "Red (nm) : "), 0, wxALIGN_CENTER_VERTICAL);
	swir_sz->AddSpacer(10);
	swir_sz->Add(mpSwirRedCtrl, wxSizerFlags().Proportion(0).Expand());
	swir_sz->AddSpacer(10);
	swir_sz->Add(new wxStaticText(this, wxID_ANY, "Green (nm) : "), 0, wxALIGN_CENTER_VERTICAL);
	swir_sz->AddSpacer(10);
	swir_sz->Add(mpSwirGreenCtrl, wxSizerFlags().Proportion(0).Expand());
	swir_sz->AddSpacer(10);
	swir_sz->Add(new wxStaticText(this, wxID_ANY, "Blue (nm) : "), 0, wxALIGN_CENTER_VERTICAL);
	swir_sz->AddSpacer(10);
	swir_sz->Add(mpSwirBlueCtrl, wxSizerFlags().Proportion(0).Expand());

	options_sizer->Add(vnir_sz, 1);
	options_sizer->AddSpacer(10);
	options_sizer->Add(swir_sz, 1);

	topsizer->Add(options_sizer, wxSizerFlags().Proportion(0).Expand());

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
	wxFileDialog dlg(this, _("Open file"), "", "",
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

	double value;
	mpVnirRedCtrl->GetValue().ToDouble(&value);
	mVnirRed_nm = static_cast<float>(value);

	mpVnirGreenCtrl->GetValue().ToDouble(&value);
	mVnirGreen_nm = static_cast<float>(value);

	mpVnirBlueCtrl->GetValue().ToDouble(&value);
	mVnirBlue_nm = static_cast<float>(value);

	mpSwirRedCtrl->GetValue().ToDouble(&value);
	mSwirRed_nm = static_cast<float>(value);

	mpSwirGreenCtrl->GetValue().ToDouble(&value);
	mSwirGreen_nm = static_cast<float>(value);

	mpSwirBlueCtrl->GetValue().ToDouble(&value);
	mSwirBlue_nm = static_cast<float>(value);

	const std::filesystem::path input{ mSource.ToStdString() };

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

	/*
	 * Add all of the files to process
	 */
	mNumFilesToProcess = 0;
	for (auto& in_file : files_to_process)
	{
		std::filesystem::path out_file;
		auto fe = removeProcessedTimestamp(in_file.path().filename().string());

//		if (mIsFile)
//		{
//			out_file = std::filesystem::path{ mDestinationDataDirectory.ToStdString()};
//		}

		if (out_file.empty())
		{
			std::string out_filename = fe.filename;
			out_file = mDestinationDataDirectory.ToStdString();
			out_file /= addProcessedTimestamp(out_filename);

			if (!fe.extension.empty())
			{
				out_file.replace_extension(fe.extension);
			}
		}

		cFileProcessor* fp = new cFileProcessor(mNumFilesToProcess++, in_file, out_file);

		fp->setVnirRgb(mVnirRed_nm, mVnirGreen_nm, mVnirBlue_nm);
		fp->setSwirRgb(mSwirRed_nm, mSwirGreen_nm, mSwirBlue_nm);

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
	msg += mSource;
	wxLogMessage(msg);

	return (wxThread::ExitCode) 0;
}

