#pragma once

#include "PointCloudTypes.hpp"
#include "PointCloud.hpp"

#include <list>
#include <vector>
#include <memory>
#include <optional>

struct sDistanceWindow_t
{
	double min_dist_m = 0.0;
	double max_dist_m = 0.0;
};

struct sAzimuthWindow_t
{
	double min_azimuth_deg = 0.0;
	double max_azimuth_deg = 0.0;
};

struct sAltitudeWindow_t
{
	double min_altitude_deg = 0.0;
	double max_altitude_deg = 0.0;
};

struct sPointCloudDimensions_t
{
	double x_min_m = 0.0;
	double x_max_m = 0.0;
	double y_min_m = 0.0;
	double y_max_m = 0.0;
	double z_min_m = 0.0;
	double z_max_m = 0.0;
};

struct sRappPoint_t
{
	std::int32_t x_mm = 0;
	std::int32_t y_mm = 0;
	std::int32_t z_mm = 0;
};


struct sPointCloudBlock
{
	pointcloud::eCOORDINATE_SYSTEM	coordinate_system = pointcloud::eCOORDINATE_SYSTEM::SENSOR_SEU;
	pointcloud::eKINEMATIC_MODEL	kinematic_model = pointcloud::eKINEMATIC_MODEL::UNKNOWN;

	std::optional<sDistanceWindow_t> distanceWindow;
	std::optional<sAzimuthWindow_t>  azimuthWindow;
	std::optional<sAltitudeWindow_t> altitudeWindow;

	std::optional<sRappPoint_t> referencePoint;

	bool vegetationOnly = false;

	std::optional<sPointCloudDimensions_t> dimensions;

	std::vector<pointcloud::sSensorKinematicInfo_t> path;

	cGenericPointCloud pointCloud;

	void clear();
};



class cPointCloudInfo : public std::enable_shared_from_this<cPointCloudInfo>
{
public:
	typedef std::list<sPointCloudBlock>  vPointClouds_t;

	typedef vPointClouds_t::value_type				value_type;
	typedef vPointClouds_t::size_type				size_type;
	typedef vPointClouds_t::difference_type			difference_type;
	typedef vPointClouds_t::reference				reference;
	typedef vPointClouds_t::const_reference			const_reference;
	typedef vPointClouds_t::pointer					pointer;
	typedef vPointClouds_t::const_pointer			const_pointer;
	typedef vPointClouds_t::iterator				iterator;
	typedef vPointClouds_t::const_iterator			const_iterator;
	typedef vPointClouds_t::reverse_iterator		reverse_iterator;
	typedef vPointClouds_t::const_reverse_iterator	const_reverse_iterator;

public:
	cPointCloudInfo() = default;
	virtual ~cPointCloudInfo() = default;

	void clear();

	bool empty() const;
	std::size_t size() const;

	const_reference front() const;

	const_iterator begin() const;
	const_iterator end() const;

protected:
	void clearPointClouds();
	void addPointCloudInfo(const sPointCloudBlock& info);

private:
	vPointClouds_t mPointClouds;

	friend class cPointCloudInfoLoader;
};

