
#pragma once

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

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
	void OnQuit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnClose(wxCloseEvent& evt);

protected:
	void OnThreadUpdate(wxThreadEvent& evt);

private:
	cMainWindow* mpMainWindow = nullptr;

	// any class wishing to process wxWidgets events must use this macro
	wxDECLARE_EVENT_TABLE();
};
