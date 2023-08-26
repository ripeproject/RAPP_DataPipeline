
#pragma once

#include <cbdf/BlockDataFile.hpp>

#include <filesystem>
#include <string>
#include <memory>

// Forward Declarations
class cPointCloud2Ply;
class wxEvtHandler;


class cFileProcessor
{
public:
	cFileProcessor(int id, std::filesystem::directory_entry in,
				std::filesystem::path out);
	~cFileProcessor();

	void setEventHandler(wxEvtHandler* handler);

	bool open(std::filesystem::path out);

	void process_file();
	void run();

private:
	const int mID;

	std::uintmax_t mFileSize;
	cBlockDataFileReader mFileReader;

	std::filesystem::path mInputFile;
	std::filesystem::path mOutputFile;

	std::unique_ptr<cPointCloud2Ply> mConverter;

	wxEvtHandler* mpEventHandler = nullptr;
};
