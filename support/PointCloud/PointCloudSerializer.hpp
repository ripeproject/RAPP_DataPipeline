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
}

class cPointCloudID;

class cReducedPointCloudByFrame;
class cSensorPointCloudByFrame;
class cPointCloud;


class cPointCloudSerializer : public cBlockSerializer
{
public:
	cPointCloudSerializer();
	explicit cPointCloudSerializer(std::size_t n, cBlockDataFileWriter* pDataFile = nullptr);
	virtual ~cPointCloudSerializer();

	void write(const pointcloud::eCOORDINATE_SYSTEM& in);
	void write(const pointcloud::imu_data_t& in);
	void write(const cReducedPointCloudByFrame& in);
	void write(const cSensorPointCloudByFrame& in);
	void write(const cPointCloud& in);

protected:
	cBlockID& blockID() override;

private:
	std::unique_ptr<cPointCloudID> mBlockID;
};

