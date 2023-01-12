
#pragma once

#include "PointCloudSerializer.hpp"

#include <cbdf/BlockDataFile.hpp>
#include <cbdf/OusterParser.hpp>
#include <ouster/simple_blas.h>

#include <filesystem>
#include <string>
#include <memory>

// Forward Declarations
class cLidar2PointCloud;


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

	std::unique_ptr<cLidar2PointCloud> mConverter;
};
