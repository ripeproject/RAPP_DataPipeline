
#pragma once

#include "PointCloudSerializer.hpp"
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

	bool open(std::filesystem::directory_entry in, 
				std::filesystem::path out);

	void process_file(std::filesystem::directory_entry in,
				std::filesystem::path out);
	void run();

private:
	void processBlock(const cBlockID& id);
	void processBlock(const cBlockID& id, const std::byte* buf, std::size_t len);

private:
	cBlockDataFileReader mFileReader;
	cBlockDataFileWriter mFileWriter;

	std::filesystem::path mInputFile;
	std::filesystem::path mOutputFile;

	Kinematics mKinematicType = Kinematics::NONE;


	std::unique_ptr<cLidar2PointCloud> mConverter;
};
