/**
 * @file
 * @brief Parser for reading the "processing info" data from a block based data file.
 * 
 * Processing Information is general information on what processing steps have been 
 * applied to the block based data file.
 */
#pragma once

#include "ProcessingInfoTypes.hpp"

#include <cbdf/BlockParser.hpp>

#include <memory>

 // Forward Declarations
class cProcessingInfoID;


class cProcessingInfoParser : public cBlockParser
{
public:
	cProcessingInfoParser();
	virtual ~cProcessingInfoParser();

	cBlockID& blockID() override;

	virtual void onProcessingInfo(processing_info::sProcessInfo_1_t info) = 0;

protected:
	void processData(BLOCK_MAJOR_VERSION_t major_version,
		             BLOCK_MINOR_VERSION_t minor_version, 
		             BLOCK_DATA_ID_t data_id, 
		             cDataBuffer& buffer) override;

protected:
	void processProcessingInfo(cDataBuffer& buffer);

private:
	std::unique_ptr<cProcessingInfoID> mBlockID;
};

