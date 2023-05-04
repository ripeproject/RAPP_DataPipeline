
#include "MainFrame.hpp"
#include "MainWindow.hpp"

#include <wx/aui/framemanager.h>
#include <wx/aui/auibook.h>
#include <wx/aboutdlg.h>
#include <wx/thread.h>

#include <cbdf/BlockDataFile.hpp>

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
wxBEGIN_EVENT_TABLE(cMainFrame, wxFrame)
	EVT_MENU(wxID_EXIT, cMainFrame::OnQuit)
	EVT_MENU(wxID_ABOUT, cMainFrame::OnAbout)
	EVT_CLOSE(cMainFrame::OnClose)
wxEND_EVENT_TABLE()



//-----------------------------------------------------------------------------
cMainFrame::cMainFrame()
	: wxFrame(NULL, wxID_ANY, "Ceres Data Repair")
{
	mpMainWindow = new cMainWindow(this);

	// set the frame icon
	SetIcon(wxICON(CeresConvert));

#if wxUSE_MENUBAR
	// create a menu bar
	wxMenu* fileMenu = new wxMenu;

	fileMenu->Append(wxID_EXIT, "E&xit\tAlt-X", "Quit this program");

	// the "About" item should be in the help menu
	wxMenu* helpMenu = new wxMenu;
	helpMenu->Append(wxID_ABOUT, "&About...", "Show about dialog");

	// now append the freshly created menu to the menu bar...
	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append(fileMenu, "&File");
	menuBar->Append(helpMenu, "&Help");

	// ... and attach this menu bar to the frame
	SetMenuBar(menuBar);

#else // !wxUSE_MENUBAR
	// If menus are not available add a button to access the about box
	wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	wxButton* aboutBtn = new wxButton(this, wxID_ANY, "About...");
	aboutBtn->Bind(wxEVT_BUTTON, &MainFrame::OnAbout, this);
	sizer->Add(aboutBtn, wxSizerFlags().Center());
	SetSizer(sizer);
#endif // wxUSE_MENUBAR/!wxUSE_MENUBAR

	// create a status bar just for fun (by default with 1 pane only)
	CreateStatusBar();

	// It is also possible to use event tables, but dynamic binding is simpler.
	Bind(wxEVT_THREAD, &cMainFrame::OnThreadUpdate, this);

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	sizer->Add(mpMainWindow, wxSizerFlags().Proportion(1).Expand().Border(wxALL, 5));

	sizer->Layout();
	SetSizerAndFit(sizer);

	SetBackgroundColour(GetMenuBar()->GetBackgroundColour());
	SetBackgroundStyle(wxBG_STYLE_SYSTEM);

	auto size = GetBestSize();
	size.IncBy(350);
	size.SetHeight(450);
	SetMinSize(size);

	Centre();
}

cMainFrame::~cMainFrame()
{
}


// event handlers
void cMainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	//stopDataProcessing();

	// true is to force the frame to close
	Close(true);
}

void cMainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxAboutDialogInfo info;
	info.SetName(wxTheApp->GetAppDisplayName());
	info.SetVersion("1.0", "0.1.0");
	info.SetDescription(_("Tries to repair damaged Ceres data formatted files.\n"));
	info.SetCopyright(wxT("Copyright (c) 2022, Carl R. Woese Institute for Genomic Biology,\n"
		"University of Illinois.\n"
		"All rights reserved.\n"));
	info.SetIcon(wxICON(ceresinfo));
	info.AddDeveloper("Brett Feddersen");
	info.SetLicense("BSD 3 - Clause License\n"
		"\n"
		"This license applies to all files in the data_repair repository and source\n"
		"distribution.This includes data_repair’s source code, the examples, and\n"
		"tests, as well as the documentation.\n"
		"\n"
		"Copyright(c) 2022, Carl R.Woese Institute for Genomic Biology\n"
		"All rights reserved.\n"
		"\n"
		"Redistribution and use in source and binary forms, with or without\n"
		"modification, are permitted provided that the following conditions are met :\n"
		"\n"
		"1. Redistributions of source code must retain the above copyright notice, this\n"
		"   list of conditions and the following disclaimer.\n"

		"2. Redistributions in binary form must reproduce the above copyright notice,\n"
		"   this list of conditions and the following disclaimer in the documentation\n"
		"   and /or other materials provided with the distribution.\n"
		"\n"
		"3. Neither the name of the copyright holder nor the names of its\n"
		"   contributors may be used to endorse or promote products derived from\n"
		"   this software without specific prior written permission.\n"
		"\n"
		"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\"\n"
		"AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE\n"
		"IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n"
		"DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE\n"
		"FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL\n"
		"DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR\n"
		"SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER\n"
		"CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,\n"
		"OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE\n"
		"OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.");

	wxAboutBox(info);
}

void cMainFrame::OnClose(wxCloseEvent&)
{
	Destroy();
}

void cMainFrame::OnThreadUpdate(wxThreadEvent& evt)
{
	SetStatusText(evt.GetString());
}

