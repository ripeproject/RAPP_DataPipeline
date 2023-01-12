/**
 * @file
 * @brief Serializer for storing point cloud data into a block based data file
 */
#pragma once

#include <cbdf/BlockSerializer.hpp>

#include <memory>


// Forward Declarations
namespace pointcloud
{
	enum class eCOORDINATE_SYSTEM : uint8_t;
	struct imu_data_t;
	struct reduced_point_cloud_by_frame_t;
	struct sensor_point_cloud_by_frame_t;
}

class cPointCloudID;
class cPointCloud;


class cPointCloudSerializer : public cBlockSerializer
{
public:
	cPointCloudSerializer();
	explicit cPointCloudSerializer(std::size_t n, cBlockDataFileWriter* pDataFile = nullptr);
	virtual ~cPointCloudSerializer();

	void write(const pointcloud::eCOORDINATE_SYSTEM& in);
	void write(const pointcloud::imu_data_t& in);
	void write(const pointcloud::reduced_point_cloud_by_frame_t& in);
	void write(const pointcloud::sensor_point_cloud_by_frame_t& in);

protected:
	cBlockID& blockID() override;

private:
	std::unique_ptr<cPointCloudID> mBlockID;
};

