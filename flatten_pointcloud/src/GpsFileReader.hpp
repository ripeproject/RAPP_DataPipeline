
#pragma once

#include "datatypes.hpp"

#include <string>
#include <vector>

// Forward Declarations


class cGpsFileReader
{
public:
	cGpsFileReader();
	explicit cGpsFileReader(const std::string& filename);

	void loadFromFile(const std::string& filename);

	const std::vector<rfm::rappPoint_t>& GetRappPoints() const;

private:
	std::vector<rfm::rappPoint_t> mRappPoints;
};

