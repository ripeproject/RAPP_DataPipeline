
#include "RappTriangle.hpp"


cRappTriangle::cRappTriangle()
{
	mVertices[0] = rfm::rappPoint_t();
	mVertices[1] = rfm::rappPoint_t();
	mVertices[2] = rfm::rappPoint_t();
}

cRappTriangle::cRappTriangle(const rfm::rappPoint_t& p1, const rfm::rappPoint_t& p2, const rfm::rappPoint_t& p3)
{
	setVertices(p1, p2, p3);
}

cRappTriangle::~cRappTriangle()
{}


void cRappTriangle::setVertices(const rfm::rappPoint_t& p1, const rfm::rappPoint_t& p2, const rfm::rappPoint_t& p3)
{
	mVertices[0] = p1;
	mVertices[1] = p2;
	mVertices[2] = p3;

	mDenominator = (p2.y_mm - p3.y_mm) * (p1.x_mm - p3.x_mm) + (p3.x_mm - p2.x_mm) * (p1.y_mm - p3.y_mm);
}

const rfm::rappPoint_t& cRappTriangle::p1() const
{
	return mVertices[0];
}

const rfm::rappPoint_t& cRappTriangle::p2() const
{
	return mVertices[1];
}

const rfm::rappPoint_t& cRappTriangle::p3() const
{
	return mVertices[2];
}

bool cRappTriangle::withinTriangle(const rfm::rappPoint_t& p) const
{
	return withinTriangle(p.x_mm, p.y_mm);
}

bool cRappTriangle::withinTriangle(double x_mm, double y_mm) const
{
	auto& p1 = mVertices[0];
	auto& p2 = mVertices[1];
	auto& p3 = mVertices[2];

	double w1 = (p2.y_mm - p3.y_mm) * (x_mm - p3.x_mm) + (p3.x_mm - p2.x_mm) * (y_mm - p3.y_mm);
	w1 /= mDenominator;

	if (w1 < 0.0) return false;

	double w2 = (p3.y_mm - p1.y_mm) * (x_mm - p3.x_mm) + (p1.x_mm - p3.x_mm) * (y_mm - p3.y_mm);
	w2 /= mDenominator;

	if (w2 < 0.0) return false;

	double w3 = 1.0 - w1 - w2;

	if (w3 < 0.0) return false;

	return true;
}

double cRappTriangle::height(const rfm::rappPoint_t& point) const
{
	return height(point.x_mm, point.y_mm);
}

double cRappTriangle::height(double x_mm, double y_mm) const
{
	auto& p1 = mVertices[0];
	auto& p2 = mVertices[1];
	auto& p3 = mVertices[2];

	double w1 = (p2.y_mm - p3.y_mm) * (x_mm - p3.x_mm) + (p3.x_mm - p2.x_mm) * (y_mm - p3.y_mm);
	w1 /= mDenominator;

	if (w1 < 0.0) return rfm::INVALID_HEIGHT;

	double w2 = (p3.y_mm - p1.y_mm) * (x_mm - p3.x_mm) + (p1.x_mm - p3.x_mm) * (y_mm - p3.y_mm);
	w2 /= mDenominator;

	if (w2 < 0.0) return rfm::INVALID_HEIGHT;

	double w3 = 1.0 - w1 - w2;

	if (w3 < 0.0) return rfm::INVALID_HEIGHT;

	return w1*p1.z_mm + w2*p2.z_mm + w3*p3.z_mm;
}

rfm::sCentroid_t cRappTriangle::centroid() const
{
	double x_mm = (mVertices[0].x_mm + mVertices[1].x_mm + mVertices[2].x_mm) / 3.0;
	double y_mm = (mVertices[0].y_mm + mVertices[1].y_mm + mVertices[2].y_mm) / 3.0;
	double z_mm = (mVertices[0].z_mm + mVertices[1].z_mm + mVertices[2].z_mm) / 3.0;

	return { x_mm, y_mm, z_mm };
}
