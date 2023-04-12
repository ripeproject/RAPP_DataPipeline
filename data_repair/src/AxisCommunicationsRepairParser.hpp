/**
 * @file
 * @brief Parser for reading Axis Communications image data from a block based data file
 */
#pragma once

#include <cbdf/AxisCommunicationsParser.hpp>

class cAxisCommunicationsRepairParser : public cAxisCommunicationsParser
{
public:
	void onActiveCameraId(int id) override;
	void onFramesPerSecond(int frames_per_sec) override;
	void onImageSize(int width, int height) override;

	void onBitmap(const cBitmapBuffer& buffer) override;
	void onJPEG(const cJpegBuffer& buffer) override;
	void onMpegFrame(const cMpegFrameBuffer& buffer) override;

protected:
	void processActiveCameraId(cDataBuffer& buffer) override;
	void processFramesPerSecond(cDataBuffer& buffer) override;
	void processBitmap(cDataBuffer& buffer) override;
	void processJPEG(cDataBuffer& buffer) override;
	void processMpegFrame(cDataBuffer& buffer) override;
	void processImageSize(cDataBuffer& buffer) override;
};


