
#pragma once

#include "ProcessingInfoSerializer.hpp"
#include "lidar2pointcloud.hpp"

#include <cbdf/BlockDataFile.hpp>
#include <cbdf/OusterParser.hpp>
#include <ouster/simple_blas.h>

#include <filesystem>
#include <string>
#include <memory>


class cFileProcessor
{
public:
	cFileProcessor(int id, std::filesystem::directory_entry in,
				std::filesystem::path out);
	~cFileProcessor();


	/**
	 * Set the valid range in meters
	 *
	 * The lidar return range is considered valid if:
	 * 		min_dist_m < range_m < max_dist_m
	 *
	 * @param	min_dist_m	: The minimum distance to be considered valid
	 * @param	max_dist_m	: The maximum distance to be considered valid
	 */
	void setValidRange_m(double min_dist_m, double max_dist_m);

	/**
	 * Set the azimuth bounds to consider valid data
	 *
	 * The azimuth angle (zero) starts at the connector and increases in the
	 * counter-clock-wise direction.
	 * 		min_azimuth_deg < azimuth_deg < max_azimuth_deg
	 *
	 * @param	min_azimuth_deg	: The minimum azimuth to be considered valid
	 * @param	max_azimuth_deg	: The maximum azimuth to be considered valid
	 */
	void setAzimuthWindow_deg(double min_azimuth_deg, double max_azimuth_deg);

	/**
	 * Set the altitude bounds to consider valid data
	 *
	 * The lidar return range is considered valid if:
	 * 		min_dist_m < range_m < max_dist_m
	 *
	 * @param	min_altitude_deg	: The minimum altitude to be considered valid
	 * @param	max_altitude_deg	: The maximum altitude to be considered valid
	 */
	void setAltitudeWindow_deg(double min_altitude_deg, double max_altitude_deg);

	/**
	 * Save aggregate point cloud
	 *
	 * An aggregate point cloud is where all of the individual point clouds generated
	 * by the individual lidar scan are combined into one point cloud.
	 */
	void saveAggregatePointCloud(bool aggregatePointCloud);

	/**
	 * Save reduced point cloud
	 *
	 * If true, all points that are outside of some bound are not stored in the data file.
	 */
	void saveReducedPointCloud(bool reducePointCloud);

	/**
	 * Set the kinematic type to apply to the pointcloud data.
	 */
	void setKinematicModel(std::unique_ptr<cKinematics> model);

	void process_file();

protected:
	bool open();
	void run();

private:
	void processBlock(const cBlockID& id);
	void processBlock(const cBlockID& id, const std::byte* buf, std::size_t len);

private:
	void deleteOutputFile();

private:
	const int mID;

	std::uintmax_t mFileSize = 0;

	cBlockDataFileReader mFileReader;
	cBlockDataFileWriter mFileWriter;

	std::filesystem::path mInputFile;
	std::filesystem::path mOutputFile;

	cProcessingInfoSerializer mSerializer;
	std::unique_ptr<cLidar2PointCloud> mConverter;
};
