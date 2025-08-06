
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

const std::optional<rfm::rappPoint_t>& cGpsFileReader::GetRefPoint() const
{
	return mRefPoint;
}

const std::vector<rfm::rappPoint_t>& cGpsFileReader::GetRappPoints() const
{
	return mRappPoints;
}

void cGpsFileReader::loadFromFile(const std::string& filename)
{
	csv2::Reader reader;

	if (!reader.mmap(filename))
	{
		return;
	}

	bool isIlStatePlane = true;

	auto header = reader.header();

	if (header.length() > 0)
	{
		auto it = header.begin();

		std::string type;
		(*it).read_value<std::string>(type);
		++it;

		if (type == "ILUC")
		{
			isIlStatePlane = true;
		}
		else if (type == "SPDR")
		{
			isIlStatePlane = false;

			std::string val;
			(*it).read_value<std::string>(val);
			auto x_mm = std::stoi(val);
			++it;
			val.clear();

			(*it).read_value<std::string>(val);
			auto y_mm = std::stoi(val);
			++it;
			val.clear();

			(*it).read_value<std::string>(val);
			auto z_mm = std::stoi(val);
			val.clear();
			
			mRefPoint = { x_mm, y_mm, z_mm };
		}
	}

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

		rfm::rappPoint_t point;
		if (isIlStatePlane)
		{
			point = rfb::fromStatePlane(northing_ft, easting_ft, elevation_ft);
		}
		else
		{
			point = { static_cast<std::int32_t>(northing_ft), static_cast<std::int32_t>(easting_ft), static_cast<std::int32_t>(elevation_ft) };
		}
		mRappPoints.emplace_back(point);
	}

}

