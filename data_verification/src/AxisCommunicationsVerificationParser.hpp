/**
 * @file
 * @brief Parser for reading Axis Communications image data from a block based data file
 */
#pragma once

#include <cbdf/AxisCommunicationsParser.hpp>

class cAxisCommunicationsVerificationParser : public cAxisCommunicationsParser
{
public:
	void onActiveCameraId(uint8_t instance_id, int id) override;
	void onFramesPerSecond(uint8_t instance_id, int frames_per_sec) override;
	void onImageSize(uint8_t instance_id, int width, int height) override;

	void onBitmap(uint8_t instance_id, const cBitmapBuffer& buffer) override;
	void onJPEG(uint8_t instance_id, const cJpegBuffer& buffer) override;
	void onMpegFrame(uint8_t instance_id, const cMpegFrameBuffer& buffer) override;

protected:
	void processActiveCameraId(cDataBuffer& buffer) override;
	void processFramesPerSecond(cDataBuffer& buffer) override;
	void processBitmap(cDataBuffer& buffer) override;
	void processJPEG(cDataBuffer& buffer) override;
	void processMpegFrame(cDataBuffer& buffer) override;
	void processImageSize(cDataBuffer& buffer) override;
};


