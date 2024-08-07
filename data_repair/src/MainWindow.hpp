
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
	void OnSourceDirectory(wxCommandEvent& event);
	void OnRepairDirectory(wxCommandEvent& event);
	void OnFailedDirectory(wxCommandEvent& event);
	void OnExperimentDirectory(wxCommandEvent& event);
	void OnRepair(wxCommandEvent& event);


protected:
	virtual wxThread::ExitCode Entry();

private:
	void startDataProcessing();
	void stopDataProcessing();

private:
	wxTextCtrl* mpSourceCtrl  = nullptr;
	wxButton* mpSourceDirButton  = nullptr;

	wxTextCtrl* mpRepairCtrl = nullptr;
	wxButton* mpRepairDirButton = nullptr;

	wxTextCtrl* mpFailedCtrl = nullptr;
	wxButton* mpFailedDirButton = nullptr;

	wxTextCtrl* mpExperimentCtrl = nullptr;
	wxButton* mpExperimentDirButton = nullptr;

	wxButton* mpRepairButton = nullptr;

	cFileProgressCtrl* mpProgressCtrl = nullptr;

	wxTextCtrl* mpLogCtrl = nullptr;
	wxLog* mpOriginalLog = nullptr;

	wxEvtHandler* mpHandler = nullptr;
	std::queue<cFileProcessor*> mFileProcessors;

	wxString mSourceDataDirectory;
	wxString mFailedDataDirectory;
	wxString mRepairedDataDirectory;
	wxString mExperimentDataDirectory;

	std::filesystem::path mTemporaryDir;

	// any class wishing to process wxWidgets events must use this macro
	wxDECLARE_EVENT_TABLE();
};
