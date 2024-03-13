
#pragma once

//#include "ProcessingInfoSerializer.hpp"
#include "PointCloudParser.hpp"
//#include "PointCloudSerializer.hpp"
#include "RappPointCloud.hpp"

#include <cbdf/BlockDataFile.hpp>

#include <filesystem>
#include <string>
#include <memory>
#include <vector>

// Forward Declarations
class cPlotBoundaries;

class cFileProcessor: public cPointCloudParser
{
public:
	cFileProcessor(int id, std::filesystem::directory_entry in,
				std::filesystem::path out);
	~cFileProcessor();

	void savePlotsInSingleFile(bool singleFile);
	void savePlyFiles(bool savePlys);
	void plyUseBinaryFormat(bool binaryFormat);

	void setPlotInfo(std::shared_ptr<cPlotBoundaries> plot_info);

	void process_file();

protected:
	bool open();
	void run();

private:
	void processBlock(const cBlockID& id);
	void processBlock(const cBlockID& id, const std::byte* buf, std::size_t len);

private:
	void onCoordinateSystem(pointcloud::eCOORDINATE_SYSTEM config_param) override;
	void onKinematicModel(pointcloud::eKINEMATIC_MODEL model) override;
	void onSensorAngles(double pitch_deg, double roll_deg, double yaw_deg) override;
	void onKinematicSpeed(double vx_mps, double vy_mps, double vz_mps) override;

	void onDimensions(double x_min_m, double x_max_m,
		double y_min_m, double y_max_m, double z_min_m, double z_max_m) override;

	void onImuData(pointcloud::imu_data_t data) override;

	void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame pointCloud) override;
	void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame_FrameId pointCloud) override;
	void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame_SensorInfo pointCloud) override;

	void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame pointCloud) override;
	void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame_FrameId pointCloud) override;
	void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame_SensorInfo pointCloud) override;

	void onPointCloudData(cPointCloud pointCloud) override;
	void onPointCloudData(cPointCloud_FrameId pointCloud) override;
	void onPointCloudData(cPointCloud_SensorInfo pointCloud) override;

private:
	void doPlotSplit();

private:
	const int mID;

	bool mSavePlotsInSingleFile = false;
	bool mSavePlyFiles = false;
	bool mPlyUseBinaryFormat = false;

	std::uintmax_t mFileSize = 0;
	double		   mFilePos = 0.0;

	cBlockDataFileReader mFileReader;
	cBlockDataFileWriter mFileWriter;

	std::filesystem::path mInputFile;
	std::filesystem::path mOutputFile;

	std::shared_ptr<cPlotBoundaries> mPlotInfo;

//	cProcessingInfoSerializer mInfoSerializer;
//	cPointCloudSerializer	  mPointCloudSerializer;

	cRappPointCloud mPointCloud;

	std::vector<std::unique_ptr<cRappPointCloud>> mPlots;
};
