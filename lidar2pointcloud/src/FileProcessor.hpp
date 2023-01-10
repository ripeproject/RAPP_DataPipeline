
#pragma once

#include "PointCloudSerializer.hpp"

#include <cbdf/BlockDataFile.hpp>
#include <cbdf/OusterParser.hpp>
#include <ouster/simple_blas.h>

#include <filesystem>
#include <string>
#include <numbers>


class cFileProcessor
{
public:
	cFileProcessor();
	~cFileProcessor();

	bool open(std::filesystem::directory_entry in, 
				std::filesystem::directory_entry out);

	void process_file(std::filesystem::directory_entry in,
				std::filesystem::directory_entry out);
	void run();

private:
	void processBlock(const cBlockID& id);
	void processBlock(const cBlockID& id, const std::byte* buf, std::size_t len);

private:
	cBlockDataFileReader mFileReader;
	cBlockDataFileWriter mFileWriter;

	std::filesystem::path mInputFile;
	std::filesystem::path mOutputFile;
};
