
#include "RappUtils.hpp"
#include "RappPointCloud.hpp"
#include "PlotSplitUtils.hpp"
#include "PointCloudUtils.hpp"

#include "Constants.hpp"

#include <cbdf/PlotPointCloud.hpp>

#include <eigen3/Eigen/Eigen>

#include <array>

namespace
{
	template<typename T>
	inline void rotate(T& x, T& y, const Eigen::Matrix3d& r)
	{
		const auto& rX = r.col(0);
		const auto& rY = r.col(1);

		double a = x * rX[0] + y * rX[1];
		double b = x * rY[0] + y * rY[1];

		x = static_cast<T>(a);
		y = static_cast<T>(b);
	}

	//-----------------------------------------------------------------------------
	void rotate(pointcloud::sBoundingBox_t& box, double angle_deg)
	{
		double angle_rad = -1.0 * angle_deg * nConstants::DEG_TO_RAD;

		Eigen::AngleAxisd rollAngle(0, Eigen::Vector3d::UnitX());
		Eigen::AngleAxisd yawAngle(angle_rad, Eigen::Vector3d::UnitZ());
		Eigen::AngleAxisd pitchAngle(0, Eigen::Vector3d::UnitY());

		Eigen::Quaternion<double> q = pitchAngle * rollAngle * yawAngle;
		Eigen::Matrix3d rotationMatrix = q.matrix();

		rotate(box.corners[0].X_m, box.corners[0].Y_m, rotationMatrix);
		rotate(box.corners[1].X_m, box.corners[1].Y_m, rotationMatrix);
		rotate(box.corners[2].X_m, box.corners[2].Y_m, rotationMatrix);
		rotate(box.corners[3].X_m, box.corners[3].Y_m, rotationMatrix);
	}

	//-----------------------------------------------------------------------------
	void translate(pointcloud::sBoundingBox_t& box, int dx_mm, int dy_mm)
	{
		double dx_m = dx_mm * nConstants::MM_TO_M;
		double dy_m = dy_mm * nConstants::MM_TO_M;

		box.corners[0].X_m += dx_m;
		box.corners[0].Y_m += dy_m;

		box.corners[1].X_m += dx_m;
		box.corners[1].Y_m += dy_m;

		box.corners[2].X_m += dx_m;
		box.corners[2].Y_m += dy_m;

		box.corners[3].X_m += dx_m;
		box.corners[3].Y_m += dy_m;
	}

	void fillPlotInformation(cRappPlot* plot, const cPlotConfigPlotInfo& info)
	{
		if (!plot) return;

		plot->setPlotName(info.getPlotName());
		plot->setEvent(info.getEvent());
		plot->setDescription(info.getDescription());
		plot->setSpecies(info.getSpecies());
		plot->setCultivar(info.getCultivar());
		plot->setConstructName(info.getConstructName());
		plot->setPotLabel(info.getPotLabel());
		plot->setSeedGeneration(info.getSeedGeneration());
		plot->setCopyNumber(info.getCopyNumber());
		plot->setLeafType(info.getLeafType());
	}
}


pointcloud::sBoundingBox_t rapp::to_bounding_box(const rfm::sPlotBoundingBox_t box)
{
	pointcloud::sBoundingBox_t result;

	// Convert spidercam coordinates (mm) to bounding box coordinates (m) 

	result.corners[0].X_m = box.northWestCorner.x_mm * nConstants::MM_TO_M;
	result.corners[0].Y_m = box.northWestCorner.y_mm * nConstants::MM_TO_M;

	result.corners[1].X_m = box.southWestCorner.x_mm * nConstants::MM_TO_M;
	result.corners[1].Y_m = box.southWestCorner.y_mm * nConstants::MM_TO_M;

	result.corners[2].X_m = box.southEastCorner.x_mm * nConstants::MM_TO_M;
	result.corners[2].Y_m = box.southEastCorner.y_mm * nConstants::MM_TO_M;

	result.corners[3].X_m = box.northEastCorner.x_mm * nConstants::MM_TO_M;
	result.corners[3].Y_m = box.northEastCorner.y_mm * nConstants::MM_TO_M;

	return result;
}

std::unique_ptr<cRappPlot> rapp::do_plot_split(const cRappPointCloud& pc, const cPlotConfigPlotInfo& plotInfo,
	const cRappPointCloud* fullPointCloud)
{
	int date = pc.date();

	std::unique_ptr<cRappPlot> plot;

	const auto* bounds = plotInfo.getBounds(date);

	if (!bounds)
		return plot;

	const auto* method = plotInfo.getIsolationMethod(date);

	if (!method)
		return plot;

	cPlotPointCloud plotPointCloud;

	switch (method->getMethod())
	{
	case ePlotIsolationMethod::NONE:
	{
		plotPointCloud = plot::isolate_basic(pc, bounds->getBoundingBox());

		if (plotPointCloud.empty())
		{
			break;
		}

		break;
	}
	case ePlotIsolationMethod::CENTER_OF_PLOT:
	{
		auto plotPointCloud = plot::isolate_center_of_plot(pc, bounds->getBoundingBox(),
			method->getPlotWidth_mm(), method->getPlotLength_mm());

		if (plotPointCloud.empty())
		{
			break;
		}

		break;
	}
	case ePlotIsolationMethod::CENTER_OF_HEIGHT:
	{
		double heightThreshold_pct = 0.0;
		
		if (method->getHeightThreshold_pct().has_value())
			heightThreshold_pct = method->getHeightThreshold_pct().value();

		double maxDisplacement_pct = 25.0;

		if (method->getMaxDisplacement_pct().has_value())
			maxDisplacement_pct = method->getMaxDisplacement_pct().value();

		if (pc.vegetationOnly())
		{
			heightThreshold_pct = 0.0;

			if (fullPointCloud)
			{
				plotPointCloud = plot::isolate_center_of_height(pc.data(), bounds->getBoundingBox(),
					fullPointCloud->data(), method->getPlotWidth_mm(), method->getPlotLength_mm(), heightThreshold_pct, maxDisplacement_pct);
			}
			else
			{
				plotPointCloud = plot::isolate_center_of_height(pc.data(), bounds->getBoundingBox(),
					method->getPlotWidth_mm(), method->getPlotLength_mm(), heightThreshold_pct, maxDisplacement_pct);
			}
		}
		else
		{
			plotPointCloud = plot::isolate_center_of_height(pc.data(), bounds->getBoundingBox(),
				method->getPlotWidth_mm(), method->getPlotLength_mm(), heightThreshold_pct, maxDisplacement_pct);
		}

		if (plotPointCloud.empty())
		{
			break;
		}

		break;
	}
	case ePlotIsolationMethod::ITERATIVE:
		break;
	case ePlotIsolationMethod::FIND_CENTER:
		break;
	case ePlotIsolationMethod::POUR:
		break;
	} // End of switch statement

	const auto* exclusions = plotInfo.getExclusions(date);

	if (exclusions)
	{
		int32_t x_mm = bounds->getBoundingBox().northWestCorner.x_mm;
		int32_t y_mm = bounds->getBoundingBox().northWestCorner.y_mm;

		remove_exclusions(plotPointCloud, x_mm, y_mm, exclusions->begin(), exclusions->end());
	}

	plot = std::make_unique<cRappPlot>(plotInfo.getPlotNumber());

	fillPlotInformation(plot.get(), plotInfo);

	plot->setPointCloud(plotPointCloud);

	return plot;
}

std::vector< std::unique_ptr<cRappPlot>> rapp::do_plot_split(const cRappPointCloud& pointCloud, cPlotConfigScan::iterator first,
	const cPlotConfigScan::iterator last, const cRappPointCloud* fullPointCloud)
{
	int date = pointCloud.date();

	std::vector<std::unique_ptr<cRappPlot>> plots;

	for (; first != last; ++first)
	{
		const auto& plotInfo = *first;

		const auto* bounds = plotInfo.getBounds(date);

		if (!bounds)
			continue;

		const auto* method = plotInfo.getIsolationMethod(date);

		if (!method)
			continue;

		cPlotPointCloud plotPointCloud;

		switch (method->getMethod())
		{
		case ePlotIsolationMethod::NONE:
		{
			plotPointCloud = plot::isolate_basic(pointCloud, bounds->getBoundingBox());

			if (plotPointCloud.empty())
			{
				break;
			}

			if (pointCloud.groundLevel_mm().has_value())
				plotPointCloud.setGroundLevel_mm(pointCloud.groundLevel_mm().value());

			break;
		}
		case ePlotIsolationMethod::CENTER_OF_PLOT:
		{
			plotPointCloud = plot::isolate_center_of_plot(pointCloud, bounds->getBoundingBox(),
				method->getPlotWidth_mm(), method->getPlotLength_mm());

			if (plotPointCloud.empty())
			{
				break;
			}

			if (pointCloud.groundLevel_mm().has_value())
				plotPointCloud.setGroundLevel_mm(pointCloud.groundLevel_mm().value());

			break;
		}
		case ePlotIsolationMethod::CENTER_OF_HEIGHT:
		{
			double heightThreshold_pct = 0.0;
			
			if (method->getHeightThreshold_pct().has_value())
				heightThreshold_pct = method->getHeightThreshold_pct().value();

			double maxDisplacement_pct = 25.0;

			if (method->getMaxDisplacement_pct().has_value())
				maxDisplacement_pct = method->getMaxDisplacement_pct().value();

			if (pointCloud.vegetationOnly())
			{
				heightThreshold_pct = 0.0;

				if (fullPointCloud)
				{
					plotPointCloud = plot::isolate_center_of_height(pointCloud.data(), bounds->getBoundingBox(),
						fullPointCloud->data(), method->getPlotWidth_mm(), method->getPlotLength_mm(), heightThreshold_pct, maxDisplacement_pct);
				}
				else
				{
					plotPointCloud = plot::isolate_center_of_height(pointCloud.data(), bounds->getBoundingBox(),
						method->getPlotWidth_mm(), method->getPlotLength_mm(), heightThreshold_pct, maxDisplacement_pct);
				}
			}
			else
			{
				plotPointCloud = plot::isolate_center_of_height(pointCloud.data(), bounds->getBoundingBox(),
					method->getPlotWidth_mm(), method->getPlotLength_mm(), heightThreshold_pct, maxDisplacement_pct);
			}

			if (plotPointCloud.empty())
			{
				break;
			}

			if (pointCloud.groundLevel_mm().has_value())
				plotPointCloud.setGroundLevel_mm(pointCloud.groundLevel_mm().value());

			break;
		}
		case ePlotIsolationMethod::ITERATIVE:
			break;
		case ePlotIsolationMethod::FIND_CENTER:
			break;
		case ePlotIsolationMethod::POUR:
			break;
		} // End of switch statement

		const auto* exclusions = plotInfo.getExclusions(date);

		if (exclusions)
		{
			int32_t x_mm = bounds->getBoundingBox().northWestCorner.x_mm;
			int32_t y_mm = bounds->getBoundingBox().northWestCorner.y_mm;

			remove_exclusions(plotPointCloud, x_mm, y_mm, exclusions->begin(), exclusions->end() );
		}

		if (!plotPointCloud.empty())
		{
			cRappPlot* plot = new cRappPlot(plotInfo.getPlotNumber());

			fillPlotInformation(plot, plotInfo);

			plot->setPointCloud(plotPointCloud);

			plots.emplace_back(plot);
		}
	}

	return plots;
}

void rapp::remove_exclusions(cPlotPointCloud& pc, int32_t x_mm, int32_t y_mm, std::vector<cPlotConfigExclusion>::const_iterator first, const std::vector<cPlotConfigExclusion>::const_iterator last)
{
	if (pc.empty())
		return;

	for (; first != last; ++first)
	{
		auto exclusion = *first;

		auto type = exclusion.getType();

		switch (type)
		{
		case ePlotExclusionType::CIRCLE:
		{
			break;
		}
		case ePlotExclusionType::OVAL:
		{
			break;
		}
		case ePlotExclusionType::SQUARE:
		{
			pointcloud::sBoundingBox_t box;

			double half_size_m = (exclusion.length_mm() / 2) * nConstants::MM_TO_M;

			box.corners[0].X_m = -half_size_m;
			box.corners[0].Y_m = -half_size_m;

			box.corners[1].X_m = +half_size_m;
			box.corners[1].Y_m = -half_size_m;

			box.corners[2].X_m = +half_size_m;
			box.corners[2].Y_m = +half_size_m;

			box.corners[3].X_m = -half_size_m;
			box.corners[3].Y_m = +half_size_m;

			rotate(box, exclusion.orientation_deg());
			translate(box, exclusion.x_mm(), exclusion.y_mm());
			translate(box, x_mm, y_mm);
			auto data = pointcloud::trim_inside(pc.data(), box);

			pc.assign(data);

			break;
		}
		case ePlotExclusionType::RECTANGLE:
		{
			pointcloud::sBoundingBox_t box;

			double half_length_m = (exclusion.length_mm() / 2) * nConstants::MM_TO_M;
			double half_width_m = (exclusion.width_mm() / 2) * nConstants::MM_TO_M;

			box.corners[0].X_m = -half_width_m;
			box.corners[0].Y_m = -half_length_m;

			box.corners[1].X_m = +half_width_m;
			box.corners[1].Y_m = -half_length_m;

			box.corners[2].X_m = +half_width_m;
			box.corners[2].Y_m = +half_length_m;

			box.corners[3].X_m = -half_width_m;
			box.corners[3].Y_m = +half_length_m;

			rotate(box, exclusion.orientation_deg());
			translate(box, exclusion.x_mm(), exclusion.y_mm());
			translate(box, x_mm, y_mm);
			auto data = pointcloud::trim_inside(pc.data(), box);

			pc.assign(data);

			break;
		}
		}
	}
}
