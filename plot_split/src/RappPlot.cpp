
#include "RappPlot.hpp"

#include <numbers>
#include <algorithm>
#include <numeric>


cRappPlot::cRappPlot(int plotId) : mID(plotId)
{}

cRappPlot::~cRappPlot()
{}


int cRappPlot::id() const { return mID; }
const std::string& cRappPlot::name() const { return mName; }
const std::string& cRappPlot::event() const { return mEvent; }
const std::string& cRappPlot::description() const { return mDescription; }


bool cRappPlot::empty() const
{
	return mCloud.empty(); 
}


void cRappPlot::setPlotName(const std::string& name)
{
	mName = name;
}

void cRappPlot::setEvent(const std::string& event)
{
	mEvent = event;
}

void cRappPlot::setDescription(const std::string& description)
{
	mDescription = description;
}


void cRappPlot::setPointCloud(const cRappPointCloud& pointCloud)
{
	mCloud.clear();

	mCloud = pointCloud;
}

rfm::sPoint3D_t cRappPlot::getPoint(int x_mm, int y_mm, int r_mm) const
{
	rfm::sPoint3D_t result = {x_mm, y_mm, rfm::INVALID_HEIGHT};

	auto lb = result;
	lb.x_mm -= r_mm;

	auto first = std::lower_bound(mCloud.begin(), mCloud.end(), lb,
		[](const rfm::sPoint3D_t& a, rfm::sPoint3D_t value)
		{
			return a.x_mm < value.x_mm;
		});

	if (first == mCloud.end())
		return result;

	auto ub = result;
	ub.x_mm += r_mm;

	auto last = std::upper_bound(mCloud.begin(), mCloud.end(), ub,
		[](const rfm::sPoint3D_t& a, rfm::sPoint3D_t value)
		{
			return a.x_mm < value.x_mm;
		});

	std::vector<rfm::sPoint3D_t>  xs(first, last);

	lb = result;
	lb.y_mm -= r_mm;

	first = std::lower_bound(xs.begin(), xs.end(), lb,
		[](const rfm::sPoint3D_t& a, rfm::sPoint3D_t value)
		{
			return a.y_mm < value.y_mm;
		});

	if (first == xs.end())
		return result;

	ub = result;
	ub.y_mm += r_mm;

	last = std::upper_bound(xs.begin(), xs.end(), ub,
		[](const rfm::sPoint3D_t& a, rfm::sPoint3D_t value)
		{
			return a.y_mm < value.y_mm;
		});

	if (first == last)
		return *first;

	std::vector<rfm::sPoint3D_t>  points(first, last);

	if (points.empty())
		return result;

	std::size_t n = 0;
	int z = 0;
	for (const auto& point : points)
	{
		if (point.z_mm != rfm::INVALID_HEIGHT)
		{
			z += point.z_mm;
			++n;
		}
	}

	if (n > 0)
		result.z_mm = z / n;

	return result;
}



