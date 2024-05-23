
#pragma once

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include "PlotConfigFile.hpp"

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
	void OnValidateSrc(wxFocusEvent& event);
	void OnSrcBrowse(wxCommandEvent& event);
	void OnDstBrowse(wxCommandEvent& event);
	void OnCfgBrowse(wxCommandEvent& event);
	void OnPlotSplit(wxCommandEvent& event);


protected:
	virtual wxThread::ExitCode Entry();

private:
	void startDataProcessing();
	void stopDataProcessing();

private:
	std::string mFilename;

	wxTextCtrl* mpSrcDirTextCtrl = nullptr;
	wxButton* mpLoadSrcButton = nullptr;

	wxTextCtrl* mpLoadDstDir = nullptr;
	wxButton* mpLoadDstButton = nullptr;

	wxTextCtrl* mpLoadConfigFile = nullptr;
	wxButton* mpLoadConfigButton = nullptr;

	wxCheckBox* mpSavePlyFiles = nullptr;
	wxCheckBox* mpPlyUseBinaryFormat = nullptr;

	wxCheckBox* mpSavePlotsInSingleFile = nullptr;
	wxCheckBox* mpEnableFrameIDs = nullptr;
	wxCheckBox* mpEnablePixelInfo = nullptr;

	wxButton* mpSplitButton = nullptr;

	cFileProgressCtrl* mpProgressCtrl = nullptr;

	wxTextCtrl* mpLogCtrl = nullptr;
	wxLog* mpOriginalLog = nullptr;

	wxEvtHandler* mpHandler = nullptr;
	std::queue<cFileProcessor*> mFileProcessors;

	wxString mSrcDirectory;
	wxString mDstDirectory;
	wxString mConfigFileName;

//	std::unique_ptr<cConfigFileData> mConfigData;
	cPlotConfigFile mConfigData;

	// any class wishing to process wxWidgets events must use this macro
	wxDECLARE_EVENT_TABLE();
};
