
#pragma once

#include <cbdf/AxisCommunicationsParser.hpp>

#include <filesystem>
#include <string>
#include <fstream>



class cExportJpegs : public cAxisCommunicationsParser
{
public:
    cExportJpegs();
	~cExportJpegs();

    void setOutputPath(std::filesystem::path out);

private:
    void onActiveCameraId(int id) override;
    void onFramesPerSecond(int frames_per_sec) override;
    void onImageSize(int width, int height) override;
    void onBitmap(const cBitmapBuffer& buffer) override;
    void onJPEG(const cJpegBuffer& buffer) override;
    void onMpegFrame(const cMpegFrameBuffer& buffer) override;

    void writeJpeg(std::filesystem::path filename);

private:
    std::filesystem::path mOutputPath;

    uint32_t    mFrameCount = 0;

    int mWidth = 0;
    int mHeight = 0;
};
