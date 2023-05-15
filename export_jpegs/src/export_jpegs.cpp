
#include "export_jpegs.hpp"

#include <iostream>


cExportJpegs::cExportJpegs() : cAxisCommunicationsParser()
{
}

cExportJpegs::~cExportJpegs()
{
}

void cExportJpegs::setOutputPath(std::filesystem::path out)
{
    mOutputPath = out;
}

void cExportJpegs::onFramesPerSecond(int frames_per_sec) {}
void cExportJpegs::onActiveCameraId(int id) {}

void cExportJpegs::onImageSize(int width, int height)
{
    mWidth = width;
    mHeight = height;
}

void cExportJpegs::onBitmap(const cBitmapBuffer& buffer) 
{}

void cExportJpegs::onJPEG(const cJpegBuffer& buffer) 
{
}

void cExportJpegs::onMpegFrame(const cMpegFrameBuffer& buffer)
{

    std::filesystem::path filename = mOutputPath;

    std::string ext = std::to_string(mFrameCount);
    ext += ".";

    ext += "jpeg";

    filename.replace_extension(ext);

    std::filebuf binary_buffer;
    binary_buffer.open(filename, std::ios::out | std::ios::binary);
    std::ostream outstream_binary(&binary_buffer);
    if (outstream_binary.fail())
        throw std::runtime_error("failed to open " + filename.string());

    outstream_binary.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    binary_buffer.close();

    ++mFrameCount;
}

void cExportJpegs::writeJpeg(std::filesystem::path filename)
{
}

