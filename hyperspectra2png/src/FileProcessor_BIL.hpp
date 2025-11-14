
#pragma once

#include "FileProcessor.hpp"


// Forward Declarations


class cFileProcessor_BIL : public cFileProcessor
{
public:
	cFileProcessor_BIL(int id, std::filesystem::directory_entry in,
				std::filesystem::path out);
	virtual ~cFileProcessor_BIL();

	bool open(std::filesystem::path out) override;

	void process_file() override;
	void run() override;

private:

private:
};
