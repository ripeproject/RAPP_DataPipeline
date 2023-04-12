

#include "AxisCommunicationsVerificationParser.hpp"
#include "ParserExceptions.hpp"

#include <stdexcept>

void cAxisCommunicationsVerificationParser::onActiveCameraId(int id)
{
    if ((id < 1) || (id > 4))
    {
        throw bdf::invalid_data("Invalid active camera id!");
    }
}

void cAxisCommunicationsVerificationParser::onFramesPerSecond(int frames_per_sec)
{
    if ((frames_per_sec < 1) || (frames_per_sec > 30))
    {
        throw bdf::invalid_data("Invalid frames per second!");
    }
}

void cAxisCommunicationsVerificationParser::onImageSize(int width, int height)
{
    bool validWidth = (width == 480) || (width == 640) || (width == 800) ||
        (width == 854) || (width == 1024) || (width == 1280) || (width == 1920);

    bool validHeight = (height == 270) || (height == 300) || (height == 360)
        || (height == 400) || (height == 450) || (height == 480) || (height == 500)
        || (height == 600) || (height == 640) || (height == 720) || (height == 768)
        || (height == 1080);

    if (!validWidth || !validHeight)
    {
        throw bdf::invalid_data("Invalid image size!");
    }
}

void cAxisCommunicationsVerificationParser::onBitmap(const cBitmapBuffer& buffer)
{}

void cAxisCommunicationsVerificationParser::onJPEG(const cJpegBuffer& buffer)
{}

void cAxisCommunicationsVerificationParser::onMpegFrame(const cMpegFrameBuffer& buffer)
{}

void cAxisCommunicationsVerificationParser::processActiveCameraId(cDataBuffer& buffer)
{
    try
    {
        cAxisCommunicationsParser::processActiveCameraId(buffer);
    }
    catch (const std::exception& e)
    {
        std::string msg = "processActiveCameraId: ";
        msg += e.what();
        throw bdf::invalid_data(msg);
    }
}

void cAxisCommunicationsVerificationParser::processFramesPerSecond(cDataBuffer& buffer)
{
    try
    {
        cAxisCommunicationsParser::processFramesPerSecond(buffer);
    }
    catch (const std::exception& e)
    {
        std::string msg = "processFramesPerSecond: ";
        msg += e.what();
        throw bdf::invalid_data(msg);
    }
}

void cAxisCommunicationsVerificationParser::processBitmap(cDataBuffer& buffer)
{
    try
    {
        cAxisCommunicationsParser::processBitmap(buffer);
    }
    catch (const std::exception& e)
    {
        std::string msg = "processBitmap: ";
        msg += e.what();
        throw bdf::invalid_data(msg);
    }
}

void cAxisCommunicationsVerificationParser::processJPEG(cDataBuffer& buffer)
{
    try
    {
        cAxisCommunicationsParser::processJPEG(buffer);
    }
    catch (const std::exception& e)
    {
        std::string msg = "processJPEG: ";
        msg += e.what();
        throw bdf::invalid_data(msg);
    }
}

void cAxisCommunicationsVerificationParser::processMpegFrame(cDataBuffer& buffer)
{
    try
    {
        cAxisCommunicationsParser::processMpegFrame(buffer);
    }
    catch (const std::exception& e)
    {
        std::string msg = "processMpegFrame: ";
        msg += e.what();
        throw bdf::invalid_data(msg);
    }
}

void cAxisCommunicationsVerificationParser::processImageSize(cDataBuffer& buffer)
{
    try
    {
        cAxisCommunicationsParser::processImageSize(buffer);
    }
    catch (const std::exception& e)
    {
        std::string msg = "processImageSize: ";
        msg += e.what();
        throw bdf::invalid_data(msg);
    }
}





