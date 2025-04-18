
#pragma once

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
	#include <wx/wx.h>
	#include <wx/valnum.h>
#endif

#include "LidarMapConfigFile.hpp"

#include "../wxCustomWidgets/FileProgressCtrl.hpp"

#include <cbdf/BlockDataFile.hpp>

#include <nlohmann/json.hpp>

#include <memory>
#include <queue>
#include <map>
#include <string>


// Forward Declarations
class cFileProcessor;


// Define a new frame type: this is going to be our main frame
class cMainWindow : public wxPanel, public wxThreadHelper
{
public:
	// ctor(s)
	explicit cMainWindow(wxWindow* parent);
	virtual ~cMainWindow();

	void CreateControls();
	void CreateLayout();

	// Event Handlers
	void OnComplete(wxCommandEvent& event);

protected:
	void OnSrcFileBrowse(wxCommandEvent& event);
	void OnSrcDirBrowse(wxCommandEvent& event);
	void OnDstBrowse(wxCommandEvent& event);
	void OnCfgFileBrowse(wxCommandEvent& event);

	void OnCompute(wxCommandEvent& event);

protected:
	virtual wxThread::ExitCode Entry();

private:
	void startDataProcessing();
	void stopDataProcessing();

private:
	std::string mFilename;

	wxTextCtrl* mpLoadSrcFile = nullptr;
	wxButton* mpSrcFileButton = nullptr;
	wxButton* mpSrcDirButton  = nullptr;

	wxTextCtrl* mpLoadDstFile = nullptr;
	wxButton* mpLoadDstButton = nullptr;

	wxTextCtrl* mpLoadCfgFile = nullptr;
	wxButton*	mpLoadCfgButton = nullptr;

	/*** Options ***/
	wxCheckBox* mpSavePlyFiles = nullptr;
	wxCheckBox* mpPlyUseBinaryFormat = nullptr;

	wxCheckBox* mpSaveCompactFile = nullptr;
	wxCheckBox* mpSaveLidarFrameIds = nullptr;
	wxCheckBox* mpSaveLidarPixelInfo = nullptr;

	wxButton* mpComputeButton = nullptr;

	cFileProgressCtrl* mpProgressCtrl = nullptr;

	wxTextCtrl* mpLogCtrl = nullptr;
	wxLog* mpOriginalLog = nullptr;

	std::queue<cFileProcessor*> mFileProcessors;

	bool mIsFile = false;
	wxString mSource;
	wxString mDestination;
	wxString mCfgFilename;

	std::unique_ptr<cLidarMapConfigFile> mConfigData;

	// any class wishing to process wxWidgets events must use this macro
	wxDECLARE_EVENT_TABLE();
};
