
#pragma once

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
	#include <wx/wx.h>
	#include <wx/valnum.h>
#endif

#include <cbdf/BlockDataFile.hpp>

#include <memory>
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
	void OnSrcBrowse(wxCommandEvent& event);
	void OnDstBrowse(wxCommandEvent& event);
	void OnModelChange(wxCommandEvent& event);
	void OnCompute(wxCommandEvent& event);

protected:
	virtual wxThread::ExitCode Entry();

private:
	void startDataProcessing();
	void stopDataProcessing();

private:
	std::string mFilename;

	wxTextCtrl* mpLoadSrcFile = nullptr;
	wxButton* mpLoadSrcButton = nullptr;

	wxTextCtrl* mpLoadDstFile = nullptr;
	wxButton* mpLoadDstButton = nullptr;

	wxComboBox* mpKinematicModel = nullptr;

	double mKM_Sensor_Vx_mmps = 0.0;
	double mKM_Sensor_Vy_mmps = 0.0;
	double mKM_Sensor_Vz_mmps = 0.0;

	wxFloatingPointValidator<double> mKM_Sensor_Vx_val;
	wxFloatingPointValidator<double> mKM_Sensor_Vy_val;
	wxFloatingPointValidator<double> mKM_Sensor_Vz_val;

	wxTextCtrl* mpKM_Sensor_Vx_mmps = nullptr;
	wxTextCtrl* mpKM_Sensor_Vy_mmps = nullptr;
	wxTextCtrl* mpKM_Sensor_Vz_mmps = nullptr;

	double mSensorPitch_deg = -90.0;
	double mSensorRoll_deg = 0.0;
	double mSensorYaw_deg = 270.0;

	wxFloatingPointValidator<double> mKM_Sensor_Pitch_val;
	wxFloatingPointValidator<double> mKM_Sensor_Roll_val;
	wxFloatingPointValidator<double> mKM_Sensor_Yaw_val;

	wxTextCtrl* mpSensorPitch_deg = nullptr;
	wxTextCtrl* mpSensorRoll_deg = nullptr;
	wxTextCtrl* mpSensorYaw_deg = nullptr;
	wxCheckBox* mpRotateSensorToSEU = nullptr;

	double mMinimumDistance_m = 0.0;
	double mMaximumDistance_m = 0.0;

	wxFloatingPointValidator<double> mMinimumDistance_val;
	wxFloatingPointValidator<double> mMaximumDistance_val;

	wxTextCtrl* mpMinimumDistance_m = nullptr;
	wxTextCtrl* mpMaximumDistance_m = nullptr;

	wxCheckBox* mpAggregatePointCloud = nullptr;
	wxCheckBox* mpSaveReducedPointCloud = nullptr;

	wxButton* mpComputeButton = nullptr;

	wxTextCtrl* mpLogCtrl = nullptr;
	wxLog* mpOriginalLog = nullptr;

	wxEvtHandler* mpHandler = nullptr;
	std::queue<cFileProcessor*> mFileProcessors;

	bool mIsFile = false;
	wxString mSourceData;
	wxString mDestinationData;

	// any class wishing to process wxWidgets events must use this macro
	wxDECLARE_EVENT_TABLE();
};
