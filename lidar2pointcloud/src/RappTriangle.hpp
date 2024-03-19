
#pragma once

#include "datatypes.hpp"

#include <array>


// Forward Declarations


class cRappTriangle
{
public:
	cRappTriangle();
	cRappTriangle(const rfm::rappPoint_t& p1, const rfm::rappPoint_t& p2, const rfm::rappPoint_t& p3);
	~cRappTriangle();

	void setVertices(const rfm::rappPoint_t& p1, const rfm::rappPoint_t& p2, const rfm::rappPoint_t& p3);

	const rfm::rappPoint_t& p1() const;
	const rfm::rappPoint_t& p2() const;
	const rfm::rappPoint_t& p3() const;

	bool withinTriangle(const rfm::rappPoint_t& point) const;
	bool withinTriangle(double x_mm, double y_mm) const;

	double height(const rfm::rappPoint_t& point) const;
	double height(double x_mm, double y_mm) const;

	rfm::sCentroid_t centroid() const;

protected:

private:
	double mDenominator = 1.0;

	std::array<rfm::rappPoint_t, 3> mVertices;
};

