/**
 * @file
 * @brief Parser for reading Axis Communications image data from a block based data file
 */
#pragma once

#include <cbdf/AxisCommunicationsParser.hpp>

class cAxisCommunicationsVerificationParser : public cAxisCommunicationsParser
{
public:
	bool sensorPresent = false;

	int mNumImages = 0;

	void onTimestamp(uint8_t device_id, uint64_t timestamp_ns) override;
	void onActiveCameraId(uint8_t device_id, int id) override;
	void onFramesPerSecond(uint8_t device_id, int frames_per_sec) override;
	void onImageSize(uint8_t device_id, int width, int height) override;
	void onMode(uint8_t device_id, int mode) override;
	void onLapseTime(uint8_t device_id, int lapse_time_ms) override;

	void onBitmap(uint8_t device_id, const cBitmapBuffer& buffer) override;
	void onJPEG(uint8_t device_id, const cJpegBuffer& buffer) override;
	void onMpegFrame(uint8_t device_id, const cMpegFrameBuffer& buffer) override;

	void onBitmap(uint8_t device_id, uint64_t timestamp_ns, const cBitmapBuffer& buffer) override;
	void onJPEG(uint8_t device_id, uint64_t timestamp_ns, const cJpegBuffer& buffer) override;
	void onMpegFrame(uint8_t device_id, uint64_t timestamp_ns, const cMpegFrameBuffer& buffer) override;

protected:
	void processTimeStamp(cDataBuffer& buffer) override;
	void processActiveCameraId(cDataBuffer& buffer) override;
	void processFramesPerSecond(cDataBuffer& buffer) override;
	void processImageSize(cDataBuffer& buffer) override;
	void processMode(cDataBuffer& buffer) override;
	void processLapseTime(cDataBuffer& buffer) override;

	void processBitmap(cDataBuffer& buffer) override;
	void processJPEG(cDataBuffer& buffer) override;
	void processMpegFrame(cDataBuffer& buffer) override;
};


