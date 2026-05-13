
#include "GpsFileReader.hpp"
#include "RappFieldBoundary.hpp"

#include <rapidcsv.h>

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
	rapidcsv::Document doc(filename, rapidcsv::LabelParams(0, -1));

	if (doc.GetColumnCount() == 0)
	{
		return;
	}

	bool isIlStatePlane = true;

	auto header = doc.GetColumnNames();

	if (header.size() > 0)
	{
		auto it = header.begin();

		std::string type = *it;
		++it;

		if (type == "ILUC")
		{
			isIlStatePlane = true;
		}
		else if (type == "SPDR")
		{
			isIlStatePlane = false;

			std::string val;
			val = *it;
			auto x_mm = std::stoi(val);
			++it;
			val.clear();

			val = *it;
			auto y_mm = std::stoi(val);
			++it;
			val.clear();

			val = *it;
			auto z_mm = std::stoi(val);
			val.clear();
			
			mRefPoint = { x_mm, y_mm, z_mm };
		}
	}

	auto c = doc.GetColumnCount();
	auto n = doc.GetRowCount();

	for (size_t i = 0; i < n; ++i)
	{
		auto values = doc.GetRow<std::string>(i);

		auto northing_ft = std::stod(values.at(1));
		auto easting_ft = std::stod(values.at(2));
		auto elevation_ft = std::stod(values.at(3));

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

