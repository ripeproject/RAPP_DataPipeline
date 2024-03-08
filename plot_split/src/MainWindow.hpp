
#pragma once

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include "../wxCustomWidgets/FileProgressCtrl.hpp"

#include <cbdf/BlockDataFile.hpp>

// Define a new frame type: this is going to be our main frame
class cMainWindow : public wxPanel
{
public:
	// ctor(s)
	cMainWindow(wxWindow* parent);
	virtual ~cMainWindow();

	void CreateControls();
	void CreateLayout();

	// event handlers (these functions should _not_ be virtual)

protected:
	void OnValidateSrc(wxFocusEvent& event);
	void OnSrcBrowse(wxCommandEvent& event);
	void OnDstBrowse(wxCommandEvent& event);
	void OnCfgBrowse(wxCommandEvent& event);

private:
	void startDataProcessing();
	void stopDataProcessing();

private:
	std::string mFilename;

	wxTextCtrl* mpLoadSrcFile = nullptr;
	wxButton* mpLoadSrcButton = nullptr;

	wxTextCtrl* mpLoadDstFile = nullptr;
	wxButton* mpLoadDstButton = nullptr;

	wxTextCtrl* mpLoadConfigFile = nullptr;
	wxButton* mpLoadConfigButton = nullptr;

	wxButton* mpSplitButton = nullptr;

	cFileProgressCtrl* mpProgressCtrl = nullptr;

	wxTextCtrl* mpLogCtrl = nullptr;
	wxLog* mpOriginalLog = nullptr;

	wxEvtHandler* mpHandler = nullptr;

	// any class wishing to process wxWidgets events must use this macro
	wxDECLARE_EVENT_TABLE();
};
