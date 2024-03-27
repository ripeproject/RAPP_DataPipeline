#pragma once

#include "PointCloudTypes.hpp"

#include <vector>
#include <memory>

struct sPointCloudBlock
{
	pointcloud::eCOORDINATE_SYSTEM	coordinate_system = pointcloud::eCOORDINATE_SYSTEM::SENSOR_SEU;
	pointcloud::eKINEMATIC_MODEL	kinematic_model = pointcloud::eKINEMATIC_MODEL::UNKNOWN;

	// Distance Window
	double min_dist_m = 0.0;
	double max_dist_m = 0.0;

	// Azimuth Window
	double min_azimuth_deg = 0.0;
	double max_azimuth_deg = 0.0;

	// Altitude Window
	double min_altitude_deg = 0.0;
	double max_altitude_deg = 0.0;

	// Point Cloud Dimensions
	double x_min_m = 0.0;
	double x_max_m = 0.0;
	double y_min_m = 0.0;
	double y_max_m = 0.0;
	double z_min_m = 0.0;
	double z_max_m = 0.0;
};

class cPointCloudInfo : public std::enable_shared_from_this<cPointCloudInfo>
{
public:
/*
	typedef std::vector<processing_info::sProcessInfo_1_t>  vProcessingSteps_t;

	typedef vProcessingSteps_t::value_type				value_type;
	typedef vProcessingSteps_t::size_type				size_type;
	typedef vProcessingSteps_t::difference_type			difference_type;
	typedef vProcessingSteps_t::reference				reference;
	typedef vProcessingSteps_t::const_reference			const_reference;
	typedef vProcessingSteps_t::pointer					pointer;
	typedef vProcessingSteps_t::const_pointer			const_pointer;
	typedef vProcessingSteps_t::iterator				iterator;
	typedef vProcessingSteps_t::const_iterator			const_iterator;
	typedef vProcessingSteps_t::reverse_iterator		reverse_iterator;
	typedef vProcessingSteps_t::const_reverse_iterator	const_reverse_iterator;
*/

public:
	cPointCloudInfo() = default;
	virtual ~cPointCloudInfo() = default;

	void clear();

	bool empty() const;
	std::size_t size() const;

//	const_iterator begin() const;
//	const_iterator end() const;

protected:
//	void addProcessingInfo(processing_info::sProcessInfo_1_t info);

private:
//	vProcessingSteps_t mProcessingSteps;

	friend class cPointCloudInfoLoader;
};

