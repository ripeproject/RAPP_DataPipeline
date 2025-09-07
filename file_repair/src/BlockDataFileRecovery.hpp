
#pragma once

#include <cbdf/BlockDataFile.hpp>

#include <cstddef>
#include <string>
#include <cstdio>
#include <fstream>
#include <map>


class cBlockDataFileRecovery : public cBlockDataFileReader
{
public:
	bool processBlock() override;

	virtual void processBlock(const cBlockID& id) = 0;
	virtual void processBlock(const cBlockID& id, const std::byte* buf, std::size_t len) = 0;

private:
	void readPayload(std::size_t len, cDataBuffer& buffer);
	uint32_t readCRC();

	bool tryToFixBlockId(const cBlockID blockID);
	bool tryToFixBlock(const cBlockID blockID, const std::size_t len);
	bool tryToFixBlock(const cBlockID blockID, const cDataBuffer buffer, const std::size_t len);

	enum class eBlockStatus { OK, BAD_CLASS_ID, BAD_MAJOR_VERSION, BAD_MINOR_VERSION, BAD_DATA_ID, BAD_PAYLOAD, BAD_CRC};
	eBlockStatus checkBlockId(const cBlockID blockID, std::size_t len);

private:
	bool fixAtBlockId(const cBlockID originalBlockID, std::size_t originalLen, eBlockStatus blockStatus);

	bool fixAtDataBuffer(const std::size_t pos, 
		const cBlockID originalBlockID, std::size_t originalLen,
		const cBlockID insertedBlockID, std::size_t insertedLen);

	bool fixAtStartPayload(const cBlockID originalBlockID, std::size_t originalLen,
							const cBlockID insertedBlockID, std::size_t insertedLen);

	bool fixAtCRC(const cBlockID originalBlockID,
		const cBlockID insertedBlockID, std::size_t insertedLen);

	bool fixAtCRC(const cBlockID originalBlockID, const cDataBuffer originalBuffer,
		std::size_t originalLen, const cBlockID insertedBlockID, std::size_t insertedLen);

	bool recoverBlockID(cBlockID& blockID, std::size_t len, eBlockStatus blockStatus);

private:
	eBlockStatus checkExperimentBlock(const cBlockID blockID, std::size_t len);
	eBlockStatus checkPvtBlock(const cBlockID blockID, std::size_t len);
	eBlockStatus checkAxisCommunicationBlock(const cBlockID blockID, std::size_t len);
	eBlockStatus checkOusterLidarBlock(const cBlockID blockID, std::size_t len);
	eBlockStatus checkSpidercamBlock(const cBlockID blockID, std::size_t len);
	eBlockStatus checkSsnxBlock(const cBlockID blockID, std::size_t len);
	eBlockStatus checkHySpexVnirBlock(const cBlockID blockID, std::size_t len);
	eBlockStatus checkHySpexSwirBlock(const cBlockID blockID, std::size_t len);
	eBlockStatus checkWeatherBlock(const cBlockID blockID, std::size_t len);

private:
	std::ifstream::pos_type mStartOfBlock;
	std::ifstream::pos_type mStartOfClassID;
	std::ifstream::pos_type mStartOfMajorVersion;
	std::ifstream::pos_type mStartOfMinorVersion;
	std::ifstream::pos_type mStartOfDataID;
	std::ifstream::pos_type mStartOfPayload;
	std::ifstream::pos_type mStartOfCRC;
};

