
#include "MainWindow.hpp"
#include "lidar2pointcloud.hpp"
#include "FileProcessor.hpp"
#include "StringUtils.hpp"

#include "Kinematics_Constant.hpp"
#include "Kinematics_Dolly.hpp"
#include "Kinematics_GPS.hpp"
#include "Kinematics_SLAM.hpp"

#include <wx/thread.h>

#include <cbdf/BlockDataFile.hpp>

void console_message(const std::string& msg)
{
	wxLogMessage(wxString(msg));
}

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
	// Custom submenu items
	ID_RECONNECT = wxID_HIGHEST + 1,

};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(cMainWindow, wxPanel)
wxEND_EVENT_TABLE()



//-----------------------------------------------------------------------------
cMainWindow::cMainWindow(wxWindow* parent)
	:
	wxPanel(parent, wxID_ANY),
	mKM_CSO_Vx_val(1, &mKM_CSO_Vx_mmps),
	mKM_CSO_Vy_val(1, &mKM_CSO_Vy_mmps),
	mKM_CSO_Vz_val(1, &mKM_CSO_Vz_mmps),
	mKM_CSO_Height_val(2, &mKM_CSO_Height_m),
	mKM_DO_Pitch_Offset_val(1, &mKM_DO_PitchOffset_deg),
	mKM_DO_Roll_Offset_val(1, &mKM_DO_RollOffset_deg),
	mSensorPitch_val(1, &mSensorPitch_deg),
	mSensorRoll_val(1, &mSensorRoll_deg),
	mSensorYaw_val(1, &mSensorYaw_deg),
	mMinimumDistance_val(1, &mMinimumDistance_m),
	mMaximumDistance_val(1, &mMaximumDistance_m),
	mMinimumAzimuth_val(2, &mMinimumAzimuth_deg),
	mMaximumAzimuth_val(2, &mMaximumAzimuth_deg),
	mMinimumAltitude_val(2, &mMinimumAltitude_deg),
	mMaximumAltitude_val(2, &mMaximumAltitude_deg)
{
	/* Constant Speed Options */
	mKM_CSO_Vx_val.SetRange(-2000.0, 2000.0);
	mKM_CSO_Vy_val.SetRange(-2000.0, 2000.0);
	mKM_CSO_Vz_val.SetRange(-2000.0, 2000.0);
	mKM_CSO_Height_val.SetRange(0.0, 10.0);

	/* Dolly Options */
	mKM_DO_Pitch_Offset_val.SetRange(-30.0, 30.0);
	mKM_DO_Roll_Offset_val.SetRange(-30.0, 30.0);

	/* General Options */
	mSensorPitch_val.SetRange(-90.0, 90.0);
	mSensorRoll_val.SetRange(-180.0, 180.0);
	mSensorYaw_val.SetRange(0.0, 360.0);

	mMinimumDistance_val.SetRange(0.0, 4.0);
	mMaximumDistance_val.SetRange(0.0, 100.0);

	mMinimumAzimuth_val.SetRange(0.0, 360.0);
	mMaximumAzimuth_val.SetRange(0.0, 360.0);

	mMinimumAltitude_val.SetRange(-25.0, 0.0);
	mMaximumAltitude_val.SetRange(0.0, 25.0);

	CreateControls();
	CreateLayout();
}

cMainWindow::~cMainWindow()
{
	delete mpOriginalLog;
	mpOriginalLog = nullptr;
}

void cMainWindow::CreateControls()
{
	mpLoadSrcFile = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(500, -1), wxTE_READONLY);

	mpSrcFileButton = new wxButton(this, wxID_ANY, "File");
	mpSrcFileButton->Bind(wxEVT_BUTTON, &cMainWindow::OnSrcFileBrowse, this);

	mpSrcDirButton = new wxButton(this, wxID_ANY, "Directory");
	mpSrcDirButton->Bind(wxEVT_BUTTON, &cMainWindow::OnSrcDirBrowse, this);

	mpLoadDstFile = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(500, -1), wxTE_READONLY);

	mpLoadDstButton = new wxButton(this, wxID_ANY, "Browse");
	mpLoadDstButton->Bind(wxEVT_BUTTON, &cMainWindow::OnDstBrowse, this);

	wxArrayString choice;
	choice.Add("Constant Speed");
	choice.Add("Dolly");
	choice.Add("GPS");
	choice.Add("SLAM");
	mpKinematicModel = new wxComboBox(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, choice, wxCB_DROPDOWN | wxCB_READONLY);
	mpKinematicModel->SetSelection(1);
	mpKinematicModel->Bind(wxEVT_TEXT, &cMainWindow::OnModelChange, this);

	mpSensorPitch_deg = new wxTextCtrl(this, wxID_ANY, "-90.0");
	mpSensorPitch_deg->SetValidator(mSensorPitch_val);

	mpSensorRoll_deg = new wxTextCtrl(this, wxID_ANY, "0.0");
	mpSensorRoll_deg->SetValidator(mSensorRoll_val);

	mpSensorYaw_deg = new wxTextCtrl(this, wxID_ANY, "270.0");
	mpSensorYaw_deg->SetValidator(mSensorYaw_val);

	mpRotateSensorToSEU = new wxCheckBox(this, wxID_ANY, "Rotate Sensor to South/East/Up Coordinates");
	mpRotateSensorToSEU->SetValue(true);

	mpMinimumDistance_m = new wxTextCtrl(this, wxID_ANY, "1.0");
	mpMinimumDistance_m->SetValidator(mMinimumDistance_val);

	mpMaximumDistance_m = new wxTextCtrl(this, wxID_ANY, "40.0");
	mpMaximumDistance_m->SetValidator(mMaximumDistance_val);

	mpMinimumAzimuth_deg = new wxTextCtrl(this, wxID_ANY, "0.0");
	mpMinimumAzimuth_deg->SetValidator(mMinimumAzimuth_val);

	mpMaximumAzimuth_deg = new wxTextCtrl(this, wxID_ANY, "360.0");
	mpMaximumAzimuth_deg->SetValidator(mMaximumAzimuth_val);

	mpMinimumAltitude_deg = new wxTextCtrl(this, wxID_ANY, "-25.0");
	mpMinimumAltitude_deg->SetValidator(mMinimumAltitude_val);

	mpMaximumAltitude_deg = new wxTextCtrl(this, wxID_ANY, "25.0");
	mpMaximumAltitude_deg->SetValidator(mMaximumAltitude_val);

	mpAggregatePointCloud = new wxCheckBox(this, wxID_ANY, "Aggregate Point Cloud");
	mpAggregatePointCloud->SetValue(true);

	mpSaveReducedPointCloud = new wxCheckBox(this, wxID_ANY, "Save Reduced Point Cloud");

	mpComputeButton = new wxButton(this, wxID_ANY, "Compute Point Cloud");
	mpComputeButton->Disable();
	mpComputeButton->Bind(wxEVT_BUTTON, &cMainWindow::OnCompute, this);

	// redirect logs from our event handlers to text control
	mpLogCtrl = new wxTextCtrl(this, wxID_ANY, wxString(), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
	mpLogCtrl->SetMinSize(wxSize(-1, 100));
	mpOriginalLog = wxLog::SetActiveTarget(new wxLogTextCtrl(mpLogCtrl));

	mpKinematicOptions = new wxStaticBoxSizer(wxVERTICAL, this);

	/////////////////////////////////////////////////////////////////
	// Build the Constant Speed Options Panel
	/////////////////////////////////////////////////////////////////
	mpKM_ConstantSpeedOptions = new wxPanel(this, wxID_ANY);

	mpKM_CSO_Vx_mmps = new wxTextCtrl(mpKM_ConstantSpeedOptions, wxID_ANY, "0.0");
	mpKM_CSO_Vx_mmps->SetValidator(mKM_CSO_Vx_val);

	mpKM_CSO_Vy_mmps = new wxTextCtrl(mpKM_ConstantSpeedOptions, wxID_ANY, "0.0");
	mpKM_CSO_Vy_mmps->SetValidator(mKM_CSO_Vy_val);

	mpKM_CSO_Vz_mmps = new wxTextCtrl(mpKM_ConstantSpeedOptions, wxID_ANY, "0.0");
	mpKM_CSO_Vz_mmps->SetValidator(mKM_CSO_Vz_val);

	mpKM_CSO_Height_m = new wxTextCtrl(mpKM_ConstantSpeedOptions, wxID_ANY, "0.0");
	mpKM_CSO_Height_m->SetValidator(mKM_CSO_Height_val);

	wxArrayString axis_choices;
	axis_choices.Add("None");
	axis_choices.Add("X");
	axis_choices.Add("Y");
	axis_choices.Add("Z");
	mpKM_CSO_HeightAxis = new wxComboBox(mpKM_ConstantSpeedOptions, wxID_ANY, "",
		wxDefaultPosition, wxDefaultSize, axis_choices, wxCB_DROPDOWN | wxCB_READONLY);
	mpKM_CSO_HeightAxis->SetSelection(0);

	createConstantSpeedLayout();

	/////////////////////////////////////////////////////////////////
	// Build the Dolly Options Panel
	/////////////////////////////////////////////////////////////////
	mpKM_DollyOptions = new wxPanel(this, wxID_ANY);

	mpKM_DO_UseImuData = new wxCheckBox(mpKM_DollyOptions, wxID_ANY, "Use IMU Data");
	mpKM_DO_UseImuData->SetValue(true);

	mpKM_DO_AverageImuData = new wxCheckBox(mpKM_DollyOptions, wxID_ANY, "Average IMU Data");

	mpKM_DO_PitchOffset_deg = new wxTextCtrl(mpKM_DollyOptions, wxID_ANY, "0.0");
	mpKM_DO_PitchOffset_deg->SetValidator(mKM_DO_Pitch_Offset_val);

	mpKM_DO_RollOffset_deg = new wxTextCtrl(mpKM_DollyOptions, wxID_ANY, "0.0");
	mpKM_DO_RollOffset_deg->SetValidator(mKM_DO_Roll_Offset_val);

	createDollyLayout();

	/////////////////////////////////////////////////////////////////
	// Build the GPS Options Panel
	/////////////////////////////////////////////////////////////////
	mpKM_GpsOptions = new wxPanel(this, wxID_ANY);

	/////////////////////////////////////////////////////////////////
	// Build the SLAM Options Panel
	/////////////////////////////////////////////////////////////////
	mpKM_SlamOptions = new wxPanel(this, wxID_ANY);
}

void cMainWindow::createConstantSpeedLayout()
{
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->AddSpacer(5);

	auto* grid_sizer = new wxFlexGridSizer(2);
	grid_sizer->SetVGap(5);
	grid_sizer->SetHGap(5);
	grid_sizer->AddGrowableCol(1, 1);

	grid_sizer->Add(new wxStaticText(mpKM_ConstantSpeedOptions, wxID_ANY, "Vx (mm/sec) :"),
		0, wxALIGN_CENTER_VERTICAL);
	grid_sizer->Add(mpKM_CSO_Vx_mmps, wxSizerFlags().Proportion(1).Expand());
	grid_sizer->Add(new wxStaticText(mpKM_ConstantSpeedOptions, wxID_ANY, "Vy (mm/sec) :"), 0, wxALIGN_CENTER_VERTICAL);
	grid_sizer->Add(mpKM_CSO_Vy_mmps, wxSizerFlags().Proportion(1).Expand());
	grid_sizer->Add(new wxStaticText(mpKM_ConstantSpeedOptions, wxID_ANY, "Vz (mm/sec) :"), 0, wxALIGN_CENTER_VERTICAL);
	grid_sizer->Add(mpKM_CSO_Vz_mmps, wxSizerFlags().Proportion(1).Expand());

	sizer->Add(grid_sizer, wxSizerFlags().Proportion(0).Expand());
	sizer->AddSpacer(5);

	wxBoxSizer* height_sz = new wxBoxSizer(wxHORIZONTAL);

	height_sz->Add(new wxStaticText(mpKM_ConstantSpeedOptions, wxID_ANY, "Height (m) :"),
		0, wxALIGN_CENTER_VERTICAL);
	height_sz->AddSpacer(5);
	height_sz->Add(mpKM_CSO_Height_m, wxSizerFlags().Proportion(1).Expand());
	height_sz->AddSpacer(5);
	height_sz->Add(new wxStaticText(mpKM_ConstantSpeedOptions, wxID_ANY, "Axis :"),
		0, wxALIGN_CENTER_VERTICAL);
	height_sz->AddSpacer(5);
	height_sz->Add(mpKM_CSO_HeightAxis, wxSizerFlags().Proportion(1).Expand());

	sizer->Add(height_sz, wxSizerFlags().Proportion(1).Expand());

	mpKM_ConstantSpeedOptions->SetSizerAndFit(sizer);
	mpKM_ConstantSpeedOptions->Hide();
}

void cMainWindow::createDollyLayout()
{
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->AddSpacer(5);

	sizer->Add(mpKM_DO_UseImuData, wxSizerFlags().Proportion(1).Expand());
	sizer->AddSpacer(5);
	sizer->Add(mpKM_DO_AverageImuData, wxSizerFlags().Proportion(1).Expand());
	sizer->AddSpacer(5);

	auto* grid_sizer = new wxFlexGridSizer(2);
	grid_sizer->SetVGap(5);
	grid_sizer->SetHGap(5);
	grid_sizer->AddGrowableCol(1, 1);

	grid_sizer->Add(new wxStaticText(mpKM_DollyOptions, wxID_ANY, "Pitch Offset (deg) :"),
		0, wxALIGN_CENTER_VERTICAL);
	grid_sizer->Add(mpKM_DO_PitchOffset_deg, wxSizerFlags().Proportion(1).Expand());

	grid_sizer->Add(new wxStaticText(mpKM_DollyOptions, wxID_ANY, "Roll Offset (deg) :"),
		0, wxALIGN_CENTER_VERTICAL);
	grid_sizer->Add(mpKM_DO_RollOffset_deg, wxSizerFlags().Proportion(1).Expand());

	sizer->Add(grid_sizer, wxSizerFlags().Proportion(0).Expand());

	mpKM_DollyOptions->SetSizerAndFit(sizer);
	mpKM_DollyOptions->Hide();
}


void cMainWindow::CreateLayout()
{
	wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
	topsizer->AddSpacer(10);

	auto* grid_sizer = new wxFlexGridSizer(4);
	grid_sizer->SetVGap(5);
	grid_sizer->SetHGap(5);
	grid_sizer->AddGrowableCol(1, 1);

	grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Source: "), 0, wxALIGN_CENTER_VERTICAL);
	grid_sizer->Add(mpLoadSrcFile, 1, wxEXPAND);
	grid_sizer->Add(mpSrcFileButton, 0, wxALIGN_CENTER_VERTICAL);
	grid_sizer->Add(mpSrcDirButton, 0, wxALIGN_CENTER_VERTICAL);

	grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Destination: "), 0, wxALIGN_CENTER_VERTICAL);
	grid_sizer->Add(mpLoadDstFile, 1, wxEXPAND);
	grid_sizer->Add(mpLoadDstButton, 0, wxALIGN_CENTER_VERTICAL);
	topsizer->Add(grid_sizer, wxSizerFlags().Proportion(0).Expand());

	topsizer->AddSpacer(5);
	auto* sz = new wxStaticBoxSizer(wxVERTICAL, this, "Kinematic Model");
	sz->Add(mpKinematicModel, wxSizerFlags().Proportion(0).Expand());
	topsizer->Add(sz, wxSizerFlags().Proportion(0).Expand());
	topsizer->AddSpacer(5);

	{
		wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add(mpKinematicOptions, wxSizerFlags().Proportion(1).Expand());

		sizer->AddSpacer(10);

		auto* so_sz = new wxStaticBoxSizer(wxVERTICAL, this, "Sensor Orientations");
		wxBoxSizer* soi_sz = new wxBoxSizer(wxVERTICAL);

		grid_sizer = new wxFlexGridSizer(2);
		grid_sizer->SetVGap(5);
		grid_sizer->SetHGap(5);
		grid_sizer->AddGrowableCol(1, 1);

		grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Pitch (deg) :"), 0, wxALIGN_CENTER_VERTICAL);
		grid_sizer->Add(mpSensorPitch_deg, wxSizerFlags().Proportion(0).Expand());
		grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Roll (deg) :"), 0, wxALIGN_CENTER_VERTICAL);
		grid_sizer->Add(mpSensorRoll_deg, wxSizerFlags().Proportion(0).Expand());
		grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Yaw (deg) :"), 0, wxALIGN_CENTER_VERTICAL);
		grid_sizer->Add(mpSensorYaw_deg, wxSizerFlags().Proportion(0).Expand());

		soi_sz->Add(grid_sizer, wxSizerFlags().Proportion(0).Expand());
		soi_sz->AddSpacer(5);
		soi_sz->Add(mpRotateSensorToSEU, wxSizerFlags().Proportion(0).Expand());

		so_sz->Add(soi_sz, wxSizerFlags().Proportion(0).Expand());
		sizer->Add(so_sz, wxSizerFlags().Proportion(1).Expand());

		topsizer->Add(sizer, wxSizerFlags().Proportion(1).Expand());
	}
	topsizer->AddSpacer(5);

	auto* limit_sz = new wxStaticBoxSizer(wxHORIZONTAL, this, "Sensor Limits");

	auto* lg_sz = new wxFlexGridSizer(4);
	lg_sz->SetVGap(5);
	lg_sz->SetHGap(5);
	lg_sz->AddGrowableCol(1, 1);
	lg_sz->AddGrowableCol(3, 1);

	lg_sz->Add(new wxStaticText(this, wxID_ANY, "Minimum Distance (m) :"), 0, wxALIGN_CENTER_VERTICAL);
	lg_sz->Add(mpMinimumDistance_m, wxSizerFlags().Proportion(1).Expand());
	lg_sz->Add(new wxStaticText(this, wxID_ANY, "Maximum Distance (m) :"), 0, wxALIGN_CENTER_VERTICAL);
	lg_sz->Add(mpMaximumDistance_m, wxSizerFlags().Proportion(1).Expand());

	lg_sz->Add(new wxStaticText(this, wxID_ANY, "Minimum Azimuth (deg) :"), 0, wxALIGN_CENTER_VERTICAL);
	lg_sz->Add(mpMinimumAzimuth_deg, wxSizerFlags().Proportion(1).Expand());
	lg_sz->Add(new wxStaticText(this, wxID_ANY, "Maximum Azimuth (deg) :"), 0, wxALIGN_CENTER_VERTICAL);
	lg_sz->Add(mpMaximumAzimuth_deg, wxSizerFlags().Proportion(1).Expand());

	lg_sz->Add(new wxStaticText(this, wxID_ANY, "Minimum Altitude (deg) :"), 0, wxALIGN_CENTER_VERTICAL);
	lg_sz->Add(mpMinimumAltitude_deg, wxSizerFlags().Proportion(1).Expand());
	lg_sz->Add(new wxStaticText(this, wxID_ANY, "Maximum Altitude (deg) :"), 0, wxALIGN_CENTER_VERTICAL);
	lg_sz->Add(mpMaximumAltitude_deg, wxSizerFlags().Proportion(1).Expand());

	limit_sz->Add(lg_sz, wxSizerFlags().Proportion(1).Expand());
	topsizer->Add(limit_sz, wxSizerFlags().Proportion(0).Expand());

	topsizer->AddSpacer(5);

	auto* op_sz = new wxStaticBoxSizer(wxHORIZONTAL, this, "Options");
	op_sz->Add(mpAggregatePointCloud, wxSizerFlags().Proportion(1).Expand());
	op_sz->AddSpacer(10);
	op_sz->Add(mpSaveReducedPointCloud, wxSizerFlags().Proportion(1).Expand());
	topsizer->Add(op_sz, wxSizerFlags().Proportion(0).Expand());

	topsizer->AddSpacer(10);
	topsizer->Add(mpComputeButton, wxSizerFlags().Proportion(0).Expand());
	topsizer->AddSpacer(5);
	topsizer->Add(mpLogCtrl, wxSizerFlags().Proportion(1).Expand());

	mpKinematicOptions->GetStaticBox()->SetLabelText("Dolly Options");
	mpKinematicOptions->Add(mpKM_DollyOptions, 1, wxGROW);
	mpKM_DollyOptions->Show();

	topsizer->Layout();

	SetSizerAndFit(topsizer);
}

// event handlers

void cMainWindow::OnSrcFileBrowse(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog dlg(this, _("Open file"), "", "",
		"Ceres files (*.ceres)|*.ceres", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;     // the user changed their mind...

	mIsFile = true;
	mSourceData = dlg.GetPath();
	mpLoadSrcFile->SetValue(mSourceData);

	if (!mpLoadDstFile->GetValue().IsEmpty())
		mpComputeButton->Enable();
}

void cMainWindow::OnSrcDirBrowse(wxCommandEvent& WXUNUSED(event))
{
	wxDirDialog dlg(NULL, "Choose input directory", "",
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;

	mIsFile = false;
	mSourceData = dlg.GetPath();
	mpLoadSrcFile->SetValue(mSourceData);

	if (!mpLoadDstFile->GetValue().IsEmpty())
		mpComputeButton->Enable();
}

void cMainWindow::OnDstBrowse(wxCommandEvent& event)
{
	wxDirDialog dlg(NULL, "Choose output directory", "",
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;

	mDestinationData = dlg.GetPath();
	mpLoadDstFile->SetValue(mDestinationData);

	if (!mpLoadSrcFile->GetValue().IsEmpty())
		mpComputeButton->Enable();
}

void cMainWindow::OnModelChange(wxCommandEvent& WXUNUSED(event))
{
	auto selection = mpKinematicModel->GetSelection();

	mpKinematicOptions->Detach(0);
	switch (selection)
	{
	case 0:
		mpKM_DollyOptions->Hide();
		mpKM_GpsOptions->Hide();
		mpKM_SlamOptions->Hide();
		mpKinematicOptions->Prepend(mpKM_ConstantSpeedOptions, 1, wxGROW);
		mpKinematicOptions->GetStaticBox()->SetLabelText("Constant Speeds Options");
		mpKM_ConstantSpeedOptions->Show();
		break;
	case 1:
		mpKM_ConstantSpeedOptions->Hide();
		mpKM_GpsOptions->Hide();
		mpKM_SlamOptions->Hide();
		mpKinematicOptions->Prepend(mpKM_DollyOptions, 1, wxGROW);
		mpKinematicOptions->GetStaticBox()->SetLabelText("Dolly Options");
		mpKM_DollyOptions->Show();
		break;
	case 2:
		mpKM_ConstantSpeedOptions->Hide();
		mpKM_DollyOptions->Hide();
		mpKM_SlamOptions->Hide();
		mpKinematicOptions->Prepend(mpKM_GpsOptions, 1, wxGROW);
		mpKinematicOptions->GetStaticBox()->SetLabelText("GPS Options");
		mpKM_GpsOptions->Show();
		break;
	case 3:
		mpKM_ConstantSpeedOptions->Hide();
		mpKM_DollyOptions->Hide();
		mpKM_GpsOptions->Hide();
		mpKinematicOptions->Prepend(mpKM_SlamOptions, 1, wxGROW);
		mpKinematicOptions->GetStaticBox()->SetLabelText("SLAM Options");
		mpKM_SlamOptions->Show();
		break;
	}

	mpKinematicOptions->Layout();
}

void cMainWindow::OnCompute(wxCommandEvent& WXUNUSED(event))
{
	using namespace std::filesystem;
	using namespace nStringUtils;

	mpMinimumDistance_m->TransferDataFromWindow();
	mpMaximumDistance_m->TransferDataFromWindow();

	cLidar2PointCloud::setValidRange_m(mMinimumDistance_m,
		mMaximumDistance_m);

	mpMinimumAzimuth_deg->TransferDataFromWindow();
	mpMaximumAzimuth_deg->TransferDataFromWindow();

	cLidar2PointCloud::setAzimuthWindow_deg(mMinimumAzimuth_deg,
		mMaximumAzimuth_deg);

	mpMinimumAltitude_deg->TransferDataFromWindow();
	mpMaximumAltitude_deg->TransferDataFromWindow();

	cLidar2PointCloud::setAltitudeWindow_deg(mMinimumAltitude_deg,
		mMaximumAltitude_deg);

	mpSensorYaw_deg->TransferDataFromWindow();
	mpSensorPitch_deg->TransferDataFromWindow();
	mpSensorRoll_deg->TransferDataFromWindow();

	if (mpAggregatePointCloud->GetValue())
	{
		cLidar2PointCloud::saveAggregatePointCloud();
	}
	else if (mpSaveReducedPointCloud->GetValue())
	{
		cLidar2PointCloud::saveReducedPointCloud();
	}

	std::string input_directory = mSourceData.ToStdString();
	std::string output_directory = mDestinationData.ToStdString();

	const std::filesystem::path input{ input_directory };

	std::vector<directory_entry> files_to_process;

	/*
	 * Create list of files to process
	 */
	if (mIsFile)
	{
		auto dir_entry = std::filesystem::directory_entry{ input };

		if (!dir_entry.is_regular_file())
			return;

		if (dir_entry.path().extension() != ".ceres")
			return;

		if (!isCeresFile(dir_entry.path().string()))
			return;

		files_to_process.push_back(dir_entry);
	}
	else
	{
		// Scan input directory for all CERES files to operate on
		for (auto const& dir_entry : std::filesystem::directory_iterator{ input })
		{
			if (!dir_entry.is_regular_file())
				continue;

			if (dir_entry.path().extension() != ".ceres")
				continue;

			if (!isCeresFile(dir_entry.path().string()))
				continue;

			files_to_process.push_back(dir_entry);
		}
	}

	/*
	 * Make sure the output directory exists
	 */
	const std::filesystem::path output{ output_directory };

	std::filesystem::directory_entry output_dir;

	if (mIsFile && output.has_extension())
	{
		output_dir = std::filesystem::directory_entry{ output.parent_path() };
	}
	else
	{
		output_dir = std::filesystem::directory_entry{ output };
	}

	if (!output_dir.exists())
	{
		std::filesystem::create_directories(output_dir);
	}


	eKinematics model;
	model = eKinematics::NONE;

	switch (mpKinematicModel->GetSelection())
	{
	case 0:
		model = eKinematics::CONSTANT;
		mpKM_CSO_Vx_mmps->TransferDataFromWindow();
		mpKM_CSO_Vy_mmps->TransferDataFromWindow();
		mpKM_CSO_Vz_mmps->TransferDataFromWindow();
		mpKM_CSO_Height_m->TransferDataFromWindow();
		break;
	case 1:
		model = eKinematics::DOLLY;
		mpKM_DO_PitchOffset_deg->TransferDataFromWindow();
		mpKM_DO_RollOffset_deg->TransferDataFromWindow();
		break;
	case 2:
		model = eKinematics::GPS;
		break;
	case 3:
		model = eKinematics::SLAM;
		break;
	default:
		return;
	}

	/*
	 * Add all of the files to process to the thread pool
	 */
	for (auto& in_file : files_to_process)
	{
		std::filesystem::path out_file;
		auto fe = removeProcessedTimestamp(in_file.path().filename().string());

//		if (mIsFile)
//		{
//			out_file = std::filesystem::path{ output_directory };
//		}

		if (out_file.empty())
		{
			std::string out_filename = fe.filename;
			out_file = output_directory;
			out_file /= addProcessedTimestamp(out_filename);

			if (!fe.extension.empty())
			{
				out_file += ".";
				out_file += fe.extension;
				//				out_file.replace_extension(fe.extension);
			}
		}

		cFileProcessor* fp = new cFileProcessor(in_file, out_file);

		std::unique_ptr<cKinematics> kinematics;

		switch (model)
		{
		case eKinematics::CONSTANT:
		{
			auto km = std::make_unique<cKinematics_Constant>(mKM_CSO_Vx_mmps,
				mKM_CSO_Vy_mmps, mKM_CSO_Vz_mmps);

			switch (mpKM_CSO_HeightAxis->GetSelection())
			{
			case 0:
				km->setHeightAxis(cKinematics_Constant::eHEIGHT_AXIS::NONE);
				break;
			case 1:
				km->setHeightAxis(cKinematics_Constant::eHEIGHT_AXIS::X);
				km->setX_Offset_m(mKM_CSO_Height_m);
				break;
			case 2:
				km->setHeightAxis(cKinematics_Constant::eHEIGHT_AXIS::Y);
				km->setY_Offset_m(mKM_CSO_Height_m);
				break;
			case 3:
				km->setHeightAxis(cKinematics_Constant::eHEIGHT_AXIS::Z);
				km->setZ_Offset_m(mKM_CSO_Height_m);
				break;
			}

			kinematics = std::move(km);
			break;
		}
		case eKinematics::DOLLY:
		{
			auto km = std::make_unique<cKinematics_Dolly>();
			km->useImuData(mpKM_DO_UseImuData->GetValue());
			km->averageImuData(mpKM_DO_AverageImuData->GetValue());
			km->setSensorPitchOffset_deg(mKM_DO_PitchOffset_deg);
			km->setSensorRollOffset_deg(mKM_DO_RollOffset_deg);
			kinematics = std::move(km);
			break;
		}
		case eKinematics::GPS:
		{
			auto km = std::make_unique<cKinematics_GPS>();
			kinematics = std::move(km);
			break;
		}
		case eKinematics::SLAM:
		{
			auto km = std::make_unique<cKinematics_SLAM>();
			kinematics = std::move(km);
			break;
		}
		}

		if (!kinematics)
		{

		}

		kinematics->rotateToSEU(mpRotateSensorToSEU->GetValue());
		kinematics->setSensorOrientation(mSensorYaw_deg, mSensorPitch_deg, mSensorRoll_deg);

		fp->setKinematicModel(std::move(kinematics));

		mFileProcessors.push(fp);
	}

	mpComputeButton->Disable();

	startDataProcessing();
}

void cMainWindow::startDataProcessing()
{
	auto* pThread = GetThread();
	if (pThread && pThread->IsRunning())
	{
		return;
	}

	// Now we can start processing the new data file
	if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
	{
		wxLogError("Could not create the worker thread!");
		return;
	}

	GetThread()->Run();
}


void cMainWindow::stopDataProcessing()
{
	// Stop the processing of any previous file
	auto* pThread = GetThread();
	if (pThread && pThread->IsRunning())
	{
		//		wxString msg = "Stopping the processing of: ";
		//		msg += mFilename;

		//		SetStatusText(msg);

		pThread->Delete();
	}
}

wxThread::ExitCode cMainWindow::Entry()
{
	while (mFileProcessors.size() > 0)
	{
		if (GetThread()->TestDestroy())
			break;

		auto* fp = mFileProcessors.front();
		fp->process_file();
		mFileProcessors.pop();
		delete fp;
	}

	wxLogMessage(wxString("LiDAR Data to Point Cloud Conversion Complete."));

	return (wxThread::ExitCode) 0;
}

