
#include "MainWindow.hpp"

#include <wx/aui/framemanager.h>
#include <wx/aui/auibook.h>
#include <wx/aboutdlg.h>
#include <wx/thread.h>

#include <cbdf/BlockDataFile.hpp>

void console_message(const std::string& msg)
{
	wxLogMessage(wxString(msg));
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
	mpLoadSrcFile = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_ALPHANUMERIC));
	mpLoadSrcFile->Bind(wxEVT_KILL_FOCUS, &cMainWindow::OnValidateSrc, this);

	mpLoadSrcButton = new wxButton(this, wxID_ANY, "Browse");
	mpLoadSrcButton->Bind(wxEVT_BUTTON, &cMainWindow::OnSrcBrowse, this);

	mpLoadDstFile = new wxTextCtrl(this, wxID_ANY);
	mpLoadDstButton = new wxButton(this, wxID_ANY, "Browse");
	mpLoadDstButton->Bind(wxEVT_BUTTON, &cMainWindow::OnDstBrowse, this);

	mpConvertButton = new wxButton(this, wxID_ANY, "Convert");

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
	grid_sizer->Add(mpLoadSrcFile, 1, wxEXPAND);
	grid_sizer->Add(mpLoadSrcButton, 0, wxALIGN_CENTER_VERTICAL);

	grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Destination: "), 0, wxALIGN_CENTER_VERTICAL);
	grid_sizer->Add(mpLoadDstFile, 1, wxEXPAND);
	grid_sizer->Add(mpLoadDstButton, 0, wxALIGN_CENTER_VERTICAL);
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

void cMainWindow::OnSrcBrowse(wxCommandEvent& WXUNUSED(event))
{
//	wxFileSelector("Choose input directory");

	wxDirDialog dlg(NULL, "Choose input directory", "",
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	dlg.ShowModal();

/*
	wxFileDialog
		openFileDialog(this, _("Open directory/file"), "", "",
			"Lidar files (*.lidar_data)|*.lidar_data", wxFD_OPEN); // | wxFD_FILE_MUST_EXIST);

	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;     // the user changed their mind...

//	stopDataProcessing();

	mFilename = openFileDialog.GetPath().ToStdString();

//	startDataProcessing();
*/
}

void cMainWindow::OnDstBrowse(wxCommandEvent& event)
{

}

/*
void cMainFrame::startDataProcessing()
{
	// Now we can start processing the new data file
	if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
	{
		wxLogError("Could not create the worker thread!");
		return;
	}

	GetThread()->Run();
}


void cMainFrame::stopDataProcessing()
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

