
#pragma once

#include <filesystem>


class cFileProcessor
{
public:
	cFileProcessor();
	virtual ~cFileProcessor();

	virtual void run() = 0;

	virtual bool complete() = 0;
};
