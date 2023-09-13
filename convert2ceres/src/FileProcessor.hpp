
#pragma once

#include <filesystem>


class cFileProcessor
{
public:
	cFileProcessor(int id);
	virtual ~cFileProcessor();

	virtual void run() = 0;

	virtual bool complete() = 0;

protected:
	const int mID;

	std::uintmax_t mFileSize = 0;
};
