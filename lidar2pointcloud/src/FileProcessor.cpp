
#include "FileProcessor.hpp"
#include "lidar2pointcloud.hpp"

#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <numbers>

cFileProcessor::cFileProcessor()
{}

cFileProcessor::~cFileProcessor()
{}

bool cFileProcessor::open(std::filesystem::directory_entry in,
							std::filesystem::directory_entry out)
{
	return false;
}

void cFileProcessor::process_file(std::filesystem::directory_entry in,
									std::filesystem::directory_entry out)
{

}

void cFileProcessor::run()
{

}

void cFileProcessor::processBlock(const cBlockID& id)
{

}

void cFileProcessor::processBlock(const cBlockID& id, const std::byte* buf, std::size_t len)
{

}
