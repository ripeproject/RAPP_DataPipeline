/**
 * @file
 * @brief Parser for reading Axis Communications image data from a block based data file
 */
#pragma once

#include <cbdf/AxisCommunicationsParser.hpp>

class cAxisCommunicationsVerificationParser : public cAxisCommunicationsParser
{
public:
	void onActiveCameraId(int id) override;
	void onFramesPerSecond(int frames_per_sec) override;

/*
	void onBitmap(const QBitmap& in) override;
	void onJPEG(const QImage& image) override;
	void onMpegFrame(const QImage& image) override;
*/
	void onImageSize(int width, int height) override;
};


