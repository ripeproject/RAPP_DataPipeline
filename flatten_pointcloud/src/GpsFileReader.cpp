
#include "GpsFileReader.hpp"
#include "RappFieldBoundary.hpp"

#include "Constants.hpp"

#include <csv2/reader.hpp>

cGpsFileReader::cGpsFileReader()
{
}

cGpsFileReader::cGpsFileReader(const std::string& filename)
{
	loadFromFile(filename);
}

const std::vector<rfm::rappPoint_t>& cGpsFileReader::GetRappPoints() const
{
	return mRappPoints;
}

void cGpsFileReader::loadFromFile(const std::string& filename)
{
	cRappFieldBoundary field;

	csv2::Reader reader;

	if (!reader.mmap(filename))
	{
		return;
	}

	auto header = reader.header();

	auto c = reader.cols();
	auto n = reader.rows();

	for (const auto& row : reader)
	{
		if (row.length() == 0) break;

		auto it = row.begin();

		std::string val;
		(*it).read_value<std::string>(val);
		++it;	// The first value is the point label, which we don't care about
		val.clear();

		(*it).read_value<std::string>(val);
		auto northing_ft = std::stod(val);
		++it;
		val.clear();

		(*it).read_value<std::string>(val);
		auto easting_ft = std::stod(val);
		++it;
		val.clear();

		(*it).read_value<std::string>(val);
		auto elevation_ft = std::stod(val);
		val.clear();

		auto point = field.toRappCoordinates(northing_ft, easting_ft, elevation_ft);

		mRappPoints.emplace_back(point);
	}

}

