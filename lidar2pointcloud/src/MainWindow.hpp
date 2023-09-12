
#pragma once

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
	#include <wx/wx.h>
	#include <wx/valnum.h>
#endif

#include "../wxCustomWidgets/FileProgressCtrl.hpp"

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

	// Event Handlers
	void OnComplete(wxCommandEvent& event);

protected:
	void OnSrcFileBrowse(wxCommandEvent& event);
	void OnSrcDirBrowse(wxCommandEvent& event);
	void OnDstBrowse(wxCommandEvent& event);
	void OnModelChange(wxCommandEvent& event);
	void OnCompute(wxCommandEvent& event);

protected:
	virtual wxThread::ExitCode Entry();

private:
	void startDataProcessing();
	void stopDataProcessing();

private:
	void createConstantSpeedLayout();
	void createDollyLayout();

private:
	std::string mFilename;

	wxTextCtrl* mpLoadSrcFile = nullptr;
	wxButton* mpSrcFileButton = nullptr;
	wxButton* mpSrcDirButton  = nullptr;

	wxTextCtrl* mpLoadDstFile = nullptr;
	wxButton* mpLoadDstButton = nullptr;

	wxComboBox* mpKinematicModel = nullptr;
	wxStaticBoxSizer* mpKinematicOptions = nullptr;

	/*** Constant Speed Options (CSO) ***/
	wxPanel* mpKM_ConstantSpeedOptions = nullptr;

	double mKM_CSO_Vx_mmps = 0.0;
	double mKM_CSO_Vy_mmps = 0.0;
	double mKM_CSO_Vz_mmps = 0.0;
	double mKM_CSO_Height_m = 0.0;

	wxFloatingPointValidator<double> mKM_CSO_Vx_val;
	wxFloatingPointValidator<double> mKM_CSO_Vy_val;
	wxFloatingPointValidator<double> mKM_CSO_Vz_val;
	wxFloatingPointValidator<double> mKM_CSO_Height_val;

	wxTextCtrl* mpKM_CSO_Vx_mmps = nullptr;
	wxTextCtrl* mpKM_CSO_Vy_mmps = nullptr;
	wxTextCtrl* mpKM_CSO_Vz_mmps = nullptr;
	wxTextCtrl* mpKM_CSO_Height_m = nullptr;
	wxComboBox* mpKM_CSO_HeightAxis = nullptr;

	/*** Dolly Options ***/
	wxPanel* mpKM_DollyOptions = nullptr;

	double mKM_DO_PitchOffset_deg = 0.0;
	double mKM_DO_RollOffset_deg = 0.0;

	wxFloatingPointValidator<double> mKM_DO_Pitch_Offset_val;
	wxFloatingPointValidator<double> mKM_DO_Roll_Offset_val;

	wxCheckBox* mpKM_DO_UseImuData = nullptr;
	wxCheckBox* mpKM_DO_AverageImuData = nullptr;
	wxTextCtrl* mpKM_DO_PitchOffset_deg = nullptr;
	wxTextCtrl* mpKM_DO_RollOffset_deg = nullptr;

	/*** GPS Options ***/
	wxPanel* mpKM_GpsOptions = nullptr;

	/*** SLAM Options ***/
	wxPanel* mpKM_SlamOptions = nullptr;

	/*** Sensor Orientation ***/
	double mSensorPitch_deg = -90.0;
	double mSensorRoll_deg = 0.0;
	double mSensorYaw_deg = 270.0;

	wxFloatingPointValidator<double> mSensorPitch_val;
	wxFloatingPointValidator<double> mSensorRoll_val;
	wxFloatingPointValidator<double> mSensorYaw_val;

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

	double mMinimumAzimuth_deg = 0.0;
	double mMaximumAzimuth_deg = 360.0;

	wxFloatingPointValidator<double> mMinimumAzimuth_val;
	wxFloatingPointValidator<double> mMaximumAzimuth_val;

	wxTextCtrl* mpMinimumAzimuth_deg = nullptr;
	wxTextCtrl* mpMaximumAzimuth_deg = nullptr;

	double mMinimumAltitude_deg = -25.0;
	double mMaximumAltitude_deg = 25.0;

	wxFloatingPointValidator<double> mMinimumAltitude_val;
	wxFloatingPointValidator<double> mMaximumAltitude_val;

	wxTextCtrl* mpMinimumAltitude_deg = nullptr;
	wxTextCtrl* mpMaximumAltitude_deg = nullptr;

	wxCheckBox* mpAggregatePointCloud = nullptr;
	wxCheckBox* mpSaveReducedPointCloud = nullptr;

	wxButton* mpComputeButton = nullptr;

	cFileProgressCtrl* mpProgressCtrl = nullptr;

	wxTextCtrl* mpLogCtrl = nullptr;
	wxLog* mpOriginalLog = nullptr;

	std::queue<cFileProcessor*> mFileProcessors;

	bool mIsFile = false;
	wxString mSourceData;
	wxString mDestinationData;

	// any class wishing to process wxWidgets events must use this macro
	wxDECLARE_EVENT_TABLE();
};
