
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
	cFileProcessor();
	~cFileProcessor();

	/**
	 * Set the kinematic type to apply to the pointcloud data.
	 */
	void setKinematicModel(std::unique_ptr<cKinematics> model);

	bool open(std::filesystem::directory_entry in, 
				std::filesystem::path out);

	void process_file(std::filesystem::directory_entry in,
				std::filesystem::path out);
	void run();

private:
	void processBlock(const cBlockID& id);
	void processBlock(const cBlockID& id, const std::byte* buf, std::size_t len);

private:
	void deleteOutputFile();

private:
	cBlockDataFileReader mFileReader;
	cBlockDataFileWriter mFileWriter;

	std::filesystem::path mInputFile;
	std::filesystem::path mOutputFile;

	cProcessingInfoSerializer mSerializer;
	std::unique_ptr<cLidar2PointCloud> mConverter;
};
