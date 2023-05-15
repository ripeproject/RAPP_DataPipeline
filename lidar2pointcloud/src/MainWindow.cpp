
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
	mKM_Sensor_Vx_val(1, &mKM_Sensor_Vx_mmps),
	mKM_Sensor_Vy_val(1, &mKM_Sensor_Vy_mmps),
	mKM_Sensor_Vz_val(1, &mKM_Sensor_Vz_mmps),
	mKM_Sensor_Pitch_val(1, &mSensorPitch_deg),
	mKM_Sensor_Roll_val(1, &mSensorRoll_deg),
	mKM_Sensor_Yaw_val(1, &mSensorYaw_deg),
	mMinimumDistance_val(3, &mMinimumDistance_m),
	mMaximumDistance_val(3, &mMaximumDistance_m)

{
//	mpHandler = GetEventHandler();

	mKM_Sensor_Vx_val.SetRange(-2000.0, 2000.0);
	mKM_Sensor_Vy_val.SetRange(-2000.0, 2000.0);
	mKM_Sensor_Vz_val.SetRange(-2000.0, 2000.0);

	mKM_Sensor_Pitch_val.SetRange(-90.0, 90.0);
	mKM_Sensor_Roll_val.SetRange(-180.0, 180.0);
	mKM_Sensor_Yaw_val.SetRange(0.0, 360.0);

	mMinimumDistance_val.SetRange(0.0, 4.0);
	mMaximumDistance_val.SetRange(5.0, 100.0);

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

	mpLoadSrcButton = new wxButton(this, wxID_ANY, "Browse");
	mpLoadSrcButton->Bind(wxEVT_BUTTON, &cMainWindow::OnSrcBrowse, this);

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

	mpKM_Sensor_Vx_mmps = new wxTextCtrl(this, wxID_ANY, "0.0");
	mpKM_Sensor_Vx_mmps->Disable();
	mpKM_Sensor_Vx_mmps->SetValidator(mKM_Sensor_Vx_val);

	mpKM_Sensor_Vy_mmps = new wxTextCtrl(this, wxID_ANY, "0.0");
	mpKM_Sensor_Vy_mmps->Disable();
	mpKM_Sensor_Vy_mmps->SetValidator(mKM_Sensor_Vy_val);

	mpKM_Sensor_Vz_mmps = new wxTextCtrl(this, wxID_ANY, "0.0");
	mpKM_Sensor_Vz_mmps->Disable();
	mpKM_Sensor_Vz_mmps->SetValidator(mKM_Sensor_Vz_val);

	mpSensorPitch_deg = new wxTextCtrl(this, wxID_ANY, "-90.0");
	mpSensorPitch_deg->SetValidator(mKM_Sensor_Pitch_val);

	mpSensorRoll_deg = new wxTextCtrl(this, wxID_ANY, "0.0");
	mpSensorRoll_deg->SetValidator(mKM_Sensor_Roll_val);

	mpSensorYaw_deg = new wxTextCtrl(this, wxID_ANY, "270.0");
	mpSensorYaw_deg->SetValidator(mKM_Sensor_Yaw_val);

	mpRotateSensorToSEU = new wxCheckBox(this, wxID_ANY, "Rotate Sensor to South/East/Up Coordinates");
	mpRotateSensorToSEU->SetValue(true);

	mpMinimumDistance_m = new wxTextCtrl(this, wxID_ANY, "1.0");
	mpMinimumDistance_m->SetValidator(mMinimumDistance_val);

	mpMaximumDistance_m = new wxTextCtrl(this, wxID_ANY, "40.0");
	mpMaximumDistance_m->SetValidator(mMaximumDistance_val);

	mpAggregatePointCloud = new wxCheckBox(this, wxID_ANY, "Aggregate Point Cloud");
	mpSaveReducedPointCloud = new wxCheckBox(this, wxID_ANY, "Save Reduced Point Cloud");

	mpComputeButton = new wxButton(this, wxID_ANY, "Compute Point Cloud");
	mpComputeButton->Disable();
	mpComputeButton->Bind(wxEVT_BUTTON, &cMainWindow::OnCompute, this);

	// redirect logs from our event handlers to text control
	mpLogCtrl = new wxTextCtrl(this, wxID_ANY, wxString(), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
	mpLogCtrl->SetMinSize(wxSize(-1, 100));
	mpOriginalLog = wxLog::SetActiveTarget(new wxLogTextCtrl(mpLogCtrl));
}

void cMainWindow::CreateLayout()
{
	wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
	topsizer->AddSpacer(10);

	auto* grid_sizer = new wxFlexGridSizer(3);
	grid_sizer->SetVGap(5);
	grid_sizer->SetHGap(5);
	grid_sizer->AddGrowableCol(1, 1);

	grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Source: "), 0, wxALIGN_CENTER_VERTICAL);
	grid_sizer->Add(mpLoadSrcFile, 1, wxEXPAND);
	grid_sizer->Add(mpLoadSrcButton, 0, wxALIGN_CENTER_VERTICAL);

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
		auto* km_sz = new wxStaticBoxSizer(wxVERTICAL, this, "Sensor Speeds");

		auto* grid_sizer = new wxFlexGridSizer(2);
		grid_sizer->SetVGap(5);
		grid_sizer->SetHGap(5);
		grid_sizer->AddGrowableCol(1, 1);

		wxBoxSizer* ln_sz = new wxBoxSizer(wxHORIZONTAL);
		grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Vx (mm/sec) :"), 0, wxALIGN_CENTER_VERTICAL);
		grid_sizer->Add(mpKM_Sensor_Vx_mmps, wxSizerFlags().Proportion(0).Expand());
		grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Vy (mm/sec) :"), 0, wxALIGN_CENTER_VERTICAL);
		grid_sizer->Add(mpKM_Sensor_Vy_mmps, wxSizerFlags().Proportion(0).Expand());
		grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Vz (mm/sec) :"), 0, wxALIGN_CENTER_VERTICAL);
		grid_sizer->Add(mpKM_Sensor_Vz_mmps, wxSizerFlags().Proportion(0).Expand());

		km_sz->Add(grid_sizer, wxSizerFlags().Proportion(0).Expand());
		sizer->Add(km_sz, wxSizerFlags().Proportion(1).Expand());

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

		topsizer->Add(sizer, wxSizerFlags().Proportion(0).Expand());
	}
	topsizer->AddSpacer(5);

	auto* rg_sz = new wxStaticBoxSizer(wxHORIZONTAL, this, "Range Limits");
	rg_sz->Add(new wxStaticText(this, wxID_ANY, "Minimum Distance (m) :"), 0, wxALIGN_CENTER_VERTICAL);
	rg_sz->AddSpacer(5);
	rg_sz->Add(mpMinimumDistance_m, wxSizerFlags().Proportion(1).Expand());
	rg_sz->AddSpacer(10);
	rg_sz->Add(new wxStaticText(this, wxID_ANY, "Maximum Distance (m) :"), 0, wxALIGN_CENTER_VERTICAL);
	rg_sz->AddSpacer(5);
	rg_sz->Add(mpMaximumDistance_m, wxSizerFlags().Proportion(1).Expand());
	topsizer->Add(rg_sz, wxSizerFlags().Proportion(0).Expand());

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

	topsizer->Layout();

	SetSizerAndFit(topsizer);

	auto size = GetBestSize();
	size.IncBy(10);
	SetMinSize(size);
}

// event handlers

void cMainWindow::OnSrcBrowse(wxCommandEvent& WXUNUSED(event))
{
	wxDirDialog dlg(NULL, "Choose input directory", "",
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;

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

	if (selection == 0)
	{
		mpKM_Sensor_Vx_mmps->Enable();
		mpKM_Sensor_Vy_mmps->Enable();
		mpKM_Sensor_Vz_mmps->Enable();
	}
	else
	{
		mpKM_Sensor_Vx_mmps->Disable();
		mpKM_Sensor_Vy_mmps->Disable();
		mpKM_Sensor_Vz_mmps->Disable();
	}
}

void cMainWindow::OnCompute(wxCommandEvent& WXUNUSED(event))
{
	using namespace std::filesystem;
	using namespace nStringUtils;

	mpMinimumDistance_m->TransferDataFromWindow();
	mpMaximumDistance_m->TransferDataFromWindow();

	cLidar2PointCloud::setValidRange_m(mMinimumDistance_m,
		mMaximumDistance_m);

	mpSensorYaw_deg->TransferDataFromWindow();
	mpSensorPitch_deg->TransferDataFromWindow();
	mpSensorRoll_deg->TransferDataFromWindow();

	cLidar2PointCloud::setSensorOrientation(mSensorYaw_deg,
		mSensorPitch_deg, mSensorRoll_deg,
		mpRotateSensorToSEU->GetValue());

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
		break;
	case 1:
		model = eKinematics::DOLLY;
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

		if (mIsFile)
		{
			out_file = std::filesystem::path{ output_directory };
		}

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

		switch (model)
		{
		case eKinematics::CONSTANT:
			mpKM_Sensor_Vx_mmps->TransferDataFromWindow();
			mpKM_Sensor_Vy_mmps->TransferDataFromWindow();
			mpKM_Sensor_Vz_mmps->TransferDataFromWindow();
			fp->setKinematicModel(std::make_unique<cKinematics_Constant>(mKM_Sensor_Vx_mmps,
				mKM_Sensor_Vy_mmps, mKM_Sensor_Vz_mmps));
			break;
		case eKinematics::DOLLY:
			fp->setKinematicModel(std::make_unique<cKinematics_Dolly>());
			break;
		case eKinematics::GPS:
			fp->setKinematicModel(std::make_unique<cKinematics_GPS>());
			break;
		case eKinematics::SLAM:
			fp->setKinematicModel(std::make_unique<cKinematics_SLAM>());
			break;
		}

		mFileProcessors.push(fp);
	}

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

