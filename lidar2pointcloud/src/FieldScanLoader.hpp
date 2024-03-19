#pragma once

#include <string>

// Forward Declarations
class cFieldScanDataModel;


class cFieldScanLoader
{
public:
	cFieldScanLoader(cFieldScanDataModel& model);
	~cFieldScanLoader() = default;

	bool loadFile(const std::string& filename);

	void run();

private:
	std::string mFilename;

	cFieldScanDataModel& mModel;
};

