
#pragma once

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include "../wxCustomWidgets/FileProgressCtrl.hpp"

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

	// event handlers (these functions should _not_ be virtual)

protected:
	void OnComplete(wxCommandEvent& event);
	void OnSourceFile(wxCommandEvent& event);
	void OnSourceDirectory(wxCommandEvent& event);
	void OnDestinationDirectory(wxCommandEvent& event);
	void OnExport(wxCommandEvent& event);


protected:
	virtual wxThread::ExitCode Entry();

private:
	void startDataProcessing();
	void stopDataProcessing();

private:
	wxTextCtrl* mpSrcCtrl  = nullptr;
	wxButton* mpSrcFileButton = nullptr;
	wxButton* mpSrcDirButton  = nullptr;

	wxTextCtrl* mpDstCtrl = nullptr;
	wxButton* mpDstDirButton = nullptr;

	wxCheckBox* mpIndividualPlyFiles = nullptr;
	wxCheckBox* mpSavePositionFile = nullptr;
	wxCheckBox* mpUseBinaryFormat = nullptr;
	wxCheckBox* mpResetOrigin = nullptr;

	wxButton* mpExportButton = nullptr;

	cFileProgressCtrl* mpProgressCtrl = nullptr;

	wxTextCtrl* mpLogCtrl = nullptr;
	wxLog* mpOriginalLog = nullptr;

	wxEvtHandler* mpHandler = nullptr;
	std::queue<cFileProcessor*> mFileProcessors;

	bool mIsFile = false;
	wxString mSource;
	wxString mDestinationDataDirectory;

	// any class wishing to process wxWidgets events must use this macro
	wxDECLARE_EVENT_TABLE();
};
