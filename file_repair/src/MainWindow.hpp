
#pragma once

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include "../wxCustomWidgets/FileProgressCtrl.hpp"

#include <cbdf/BlockDataFile.hpp>

#include <queue>
#include <filesystem>

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
	void OnFailedDirectory(wxCommandEvent& event);
	void OnPartialRepairDirectory(wxCommandEvent& event);
	void OnFullRepairDirectory(wxCommandEvent& event);
	void OnRepair(wxCommandEvent& event);


protected:
	virtual wxThread::ExitCode Entry();

private:
	void startDataProcessing();
	void stopDataProcessing();

private:
	wxTextCtrl* mpFailedCtrl  = nullptr;
	wxButton* mpFailedDirButton  = nullptr;

	wxTextCtrl* mpPartialRepairCtrl = nullptr;
	wxButton* mpPartialRepairDirButton = nullptr;

	wxTextCtrl* mpFullRepairCtrl = nullptr;
	wxButton* mpFullRepairDirButton = nullptr;

	wxButton* mpRepairButton = nullptr;

	cFileProgressCtrl* mpProgressCtrl = nullptr;

	wxTextCtrl* mpLogCtrl = nullptr;
	wxLog* mpOriginalLog = nullptr;

	wxEvtHandler* mpHandler = nullptr;
	std::queue<cFileProcessor*> mFileProcessors;

	wxString mFailedDataDirectory;
	wxString mPartialRepairedDataDirectory;
	wxString mFullyRepairedDataDirectory;

	std::filesystem::path mTemporaryDir;

	// any class wishing to process wxWidgets events must use this macro
	wxDECLARE_EVENT_TABLE();
};
