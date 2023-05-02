
#include "MainWindow.hpp"
#include "FileProcessor.hpp"

#include <wx/aui/framemanager.h>
#include <wx/thread.h>

#include <cbdf/BlockDataFile.hpp>

#include <filesystem>

using namespace std::filesystem;


void console_message(const std::string& msg)
{
	wxLogMessage(wxString(msg));
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
}

void cMainWindow::CreateControls()
{
	mpSourceCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_ALPHANUMERIC));
	mpSourceCtrl->Bind(wxEVT_KILL_FOCUS, &cMainWindow::OnValidateSrc, this);

	mpSrcFileButton = new wxButton(this, wxID_ANY, "File");
	mpSrcFileButton->Bind(wxEVT_BUTTON, &cMainWindow::OnSrcFile, this);

	mpSrcDirButton = new wxButton(this, wxID_ANY, "Directory");
	mpSrcDirButton->Bind(wxEVT_BUTTON, &cMainWindow::OnSrcDirectory, this);

	mpDstCtrl = new wxTextCtrl(this, wxID_ANY);
	mpDstButton = new wxButton(this, wxID_ANY, "Browse");
	mpDstButton->Bind(wxEVT_BUTTON, &cMainWindow::OnDstDirectory, this);

	mpConvertButton = new wxButton(this, wxID_ANY, "Convert");
	mpConvertButton->Bind(wxEVT_BUTTON, &cMainWindow::OnConvert, this);

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

	if (mpDstCtrl->GetValue().IsEmpty())
	{
		path dst(mSource);
		dst.replace_extension("ceres");
		mpDstCtrl->SetValue(dst.string());
	}
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
}

void cMainWindow::OnConvert(wxCommandEvent& WXUNUSED(event))
{

}

void cMainWindow::startDataProcessing()
{
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
		wxString msg = "Stopping the processing of: ";
		msg += mFilename;

		SetStatusText(msg);

		pThread->Delete();
	}
}

wxThread::ExitCode cMainWindow::Entry()
{
	while (mFileProcessors.size() > 0)
	{
		auto* fp = mFileProcessors.front();
		fp->process_file();
	}

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

