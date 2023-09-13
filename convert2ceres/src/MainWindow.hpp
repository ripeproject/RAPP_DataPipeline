
#pragma once

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
	#include <wx/wx.h>
	#include <wx/valnum.h>
#endif

#include "../support/wxCustomWidgets/FileProgressCtrl.hpp"

#include <cbdf/BlockDataFile.hpp>

#include <queue>

// Forward Declarations
class cFileProcessor;


// Define a new frame type: this is going to be our main frame
class cMainWindow : public wxPanel, public wxThreadHelper
{
public:
	// ctor(s)
	cMainWindow(wxWindow* parent);
	virtual ~cMainWindow();

	void CreateControls();
	void CreateLayout();

	// Event Handlers
	void OnComplete(wxCommandEvent& event);

protected:
	void OnValidateSrc(wxFocusEvent& event);
	void OnSrcFile(wxCommandEvent& event);
	void OnSrcDirectory(wxCommandEvent& event);
	void OnDstDirectory(wxCommandEvent& event);
	void OnConvert(wxCommandEvent& event);


protected:
	virtual wxThread::ExitCode Entry();

private:
	void startDataProcessing();
	void stopDataProcessing();

private:
	bool mSourceIsFile = false;
	std::string mSource;

	wxTextCtrl* mpSourceCtrl  = nullptr;
	wxButton* mpSrcFileButton = nullptr;
	wxButton* mpSrcDirButton  = nullptr;

	wxTextCtrl* mpDstCtrl = nullptr;
	wxButton* mpDstButton = nullptr;

	wxButton* mpConvertButton = nullptr;

	cFileProgressCtrl* mpProgressCtrl = nullptr;

	wxTextCtrl* mpLogCtrl = nullptr;
	wxLog* mpOriginalLog = nullptr;

	wxEvtHandler* mpHandler = nullptr;
	std::queue<cFileProcessor*> mFileProcessors;


	// any class wishing to process wxWidgets events must use this macro
	wxDECLARE_EVENT_TABLE();
};
