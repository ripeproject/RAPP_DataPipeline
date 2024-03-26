
#include "MainFrame.hpp"
#include "MainWindow.hpp"
#include "GroundModelUtils.hpp"

#ifndef wxHAS_IMAGES_IN_RESOURCES
#include "Resources/LidarConvert.xpm"
#endif

#include <wx/aui/framemanager.h>
#include <wx/aui/auibook.h>
#include <wx/aboutdlg.h>
#include <wx/thread.h>
#include <wx/display.h>
#include <wx/config.h>

#include <cbdf/BlockDataFile.hpp>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif
// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
	// Custom submenu items
	ID_LOAD_GROUND_DATA = wxID_HIGHEST + 1,

};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(cMainFrame, wxFrame)
	EVT_MENU(wxID_EXIT, cMainFrame::OnQuit)
	EVT_MENU(ID_LOAD_GROUND_DATA, cMainFrame::OnFileGroundData)
	EVT_MENU(wxID_ABOUT, cMainFrame::OnAbout)
	EVT_CLOSE(cMainFrame::OnClose)
wxEND_EVENT_TABLE()



//-----------------------------------------------------------------------------
cMainFrame::cMainFrame()
	: wxFrame(NULL, wxID_ANY, "Ceres LiDAR-to-PointCloud")
{
	mMainWindow = std::make_unique<cMainWindow>(this);

	// set the frame icon
	SetIcon(wxICON(LidarConvert));

#if wxUSE_MENUBAR
	// create a menu bar
	wxMenu* fileMenu = new wxMenu();

	wxMenuItem* setting_ground_data = fileMenu->Append(ID_LOAD_GROUND_DATA, "Load Ground Data", "Load the GPS based ground data");
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_EXIT, "E&xit\tAlt-X", "Quit this program");

	// the "About" item should be in the help menu
	wxMenu* helpMenu = new wxMenu();
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

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	sizer->Add(mMainWindow.get(), wxSizerFlags().Proportion(1).Expand().Border(wxALL, 5));

	sizer->Layout();
	SetSizerAndFit(sizer);

	SetBackgroundColour(GetMenuBar()->GetBackgroundColour());
	SetBackgroundStyle(wxBG_STYLE_SYSTEM);

	auto size = GetBestSize();
	size.IncBy(50);
	size.SetWidth(1000);

	auto width = (wxDisplay().GetGeometry().GetWidth() * 3) / 4;
	auto height = (wxDisplay().GetGeometry().GetHeight() * 3) / 4;

	if (size.GetHeight() > height)
		size.SetHeight(height);

	if (size.GetWidth() > width)
		size.SetWidth(width);

	SetMinSize(size);

	Centre();

	std::unique_ptr<wxConfig> config = std::make_unique<wxConfig>("LidarConvert");
	wxString savedFileName;
	if (config->Read("Field/groundMeshFile", &savedFileName))
	{
		load_ground_data(savedFileName.ToStdString());
	}
}

cMainFrame::~cMainFrame()
{
}


// event handlers

void cMainFrame::OnFileGroundData(wxCommandEvent& WXUNUSED(event))
{
	std::unique_ptr<wxConfig> config = std::make_unique<wxConfig>("LidarConvert");
	wxString savedFileName;

	config->Read("Field/groundMeshFile", &savedFileName);
	
	wxFileDialog dlg(this, _("Import Ground Data..."), savedFileName, "",
		"GPS CSV Files (*.csv)|*.csv", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;     // the user changed their mind...

	auto ground_data = dlg.GetPath();

	if (load_ground_data(ground_data.ToStdString()))
	{
		config->Write("Field/groundMeshFile", ground_data);
	}
	else
	{

	}
}

void cMainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{

	// true is to force the frame to close
	Close(true);
}

void cMainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxAboutDialogInfo info;
	info.SetName(wxTheApp->GetAppDisplayName());
	info.SetVersion("1.0", "0.1.0");
	info.SetDescription(_("Convert LiDAR data to point cloud data in a Ceres data formatted file.     \n"));
	info.SetCopyright(wxT("Copyright (c) 2023, Carl R. Woese Institute for Genomic Biology,\n"
		"University of Illinois.\n"
		"All rights reserved.\n"));
	info.SetIcon(wxICON(LidarConvert));
	info.AddDeveloper("Brett Feddersen");
	info.SetLicense("BSD 3 - Clause License\n"
		"\n"
		"This license applies to all files in the \"lidar2pointcloud\" repository and source\n"
		"distribution. This includes LidarConvert’s source code, the examples, and\n"
		"tests, as well as the documentation.\n"
		"\n"
		"Copyright(c) 2023, Carl R.Woese Institute for Genomic Biology\n"
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


