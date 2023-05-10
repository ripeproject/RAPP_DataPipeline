/**
 * @file
 * @brief Serializer for storing "procesing info" data into a block based data file
 */
#pragma once

#include "ProcessingInfoTypes.hpp"

#include <cbdf/BlockSerializer.hpp>

#include <memory>


// Forward Declarations
class cProcessingInfoID;


class cProcessingInfoSerializer : public cBlockSerializer
{
public:
	cProcessingInfoSerializer();
	explicit cProcessingInfoSerializer(std::size_t n, cBlockDataFileWriter* pDataFile = nullptr);
	virtual ~cProcessingInfoSerializer();

	void write(const std::string& name, processing_info::ePROCESSING_TYPE type);
	void write(const processing_info::sProcessInfo_1_t& info);

protected:
	cBlockID& blockID() override;

private:
	std::unique_ptr<cProcessingInfoID> mBlockID;
};

