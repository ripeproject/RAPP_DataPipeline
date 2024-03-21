
#pragma once

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <cbdf/BlockDataFile.hpp>

#include <memory>

// Forward Declarations
class cMainWindow;


// Define a new frame type: this is going to be our main frame
class cMainFrame : public wxFrame
{
public:
	// ctor(s)
	cMainFrame();
	virtual ~cMainFrame();

	// event handlers (these functions should _not_ be virtual)
	void OnFileGroundData(wxCommandEvent& event);
	void OnQuit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnClose(wxCloseEvent& evt);

private:
	std::unique_ptr<cMainWindow> mMainWindow;

	// any class wishing to process wxWidgets events must use this macro
	wxDECLARE_EVENT_TABLE();
};
