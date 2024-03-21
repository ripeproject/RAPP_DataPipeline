#pragma once

#include <string>

// Forward Declarations
class cFieldScanDataModel;


class cFieldScanLoader
{
public:
	cFieldScanLoader(int id, cFieldScanDataModel& model);
	~cFieldScanLoader() = default;

	bool loadFile(const std::string& filename);

	bool run();

private:
	const int mID;
	std::string mFilename;

	cFieldScanDataModel& mModel;
};

