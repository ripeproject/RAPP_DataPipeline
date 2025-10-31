
#pragma once

#include <cbdf/BlockDataFile.hpp>

#include <filesystem>
#include <string>
#include <memory>

// Forward Declarations
class cExportJpegs;
class cPlotConfigFile;


class cFileProcessor
{
public:
	cFileProcessor(int id, std::filesystem::directory_entry in,
				std::filesystem::path out);
	~cFileProcessor();

	void setPlotFile(std::shared_ptr<cPlotConfigFile>& plot_file);

	void usePlotFilename(bool enable);
	void usePlotPrefix(bool enable);

	bool open(std::filesystem::path out);

	void process_file();
	void run();

private:
	const int mID;

	std::uintmax_t mFileSize = 0;
	cBlockDataFileReader mFileReader;

	std::filesystem::path mInputFile;
	std::filesystem::path mOutputFile;

	std::unique_ptr<cExportJpegs> mConverter;
};
