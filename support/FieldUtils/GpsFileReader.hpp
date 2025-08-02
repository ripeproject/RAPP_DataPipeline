
#pragma once

#include "datatypes.hpp"

#include <string>
#include <vector>
#include <optional>

// Forward Declarations


class cGpsFileReader
{
public:
	cGpsFileReader();
	explicit cGpsFileReader(const std::string& filename);

	void loadFromFile(const std::string& filename);

	const std::optional<rfm::rappPoint_t>& GetRefPoint() const;

	const std::vector<rfm::rappPoint_t>& GetRappPoints() const;

private:
	std::optional<rfm::rappPoint_t> mRefPoint;
	std::vector<rfm::rappPoint_t> mRappPoints;
};

