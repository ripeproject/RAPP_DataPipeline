
#pragma once

#include <filesystem>


class cFileProcessor
{
public:
	cFileProcessor();
	virtual ~cFileProcessor();

	virtual void process_file(std::filesystem::directory_entry in,
				std::filesystem::path out) = 0;

	virtual bool complete() = 0;

protected:
	std::filesystem::path mInputFile;
	std::filesystem::path mOutputFile;
};
