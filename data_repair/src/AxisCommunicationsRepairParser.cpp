

#include "AxisCommunicationsRepairParser.hpp"
#include "ParserExceptions.hpp"

#include <stdexcept>

cAxisCommunicationsRepairParser::cAxisCommunicationsRepairParser()
{
    mSerializer.setBufferCapacity(256 * 1024 * 1024);
}

void cAxisCommunicationsRepairParser::attach(cBlockDataFileWriter* pDataFile)
{
    mSerializer.attach(pDataFile);
}

cBlockDataFileWriter* cAxisCommunicationsRepairParser::detach()
{
    return mSerializer.detach();
}

void cAxisCommunicationsRepairParser::onActiveCameraId(uint8_t instance_id, int id)
{
    if ((id < 1) || (id > 4))
    {
        throw bdf::invalid_data("Invalid active camera id!");
    }

    mSerializer.writeActiveCameraId(instance_id, id);
}

void cAxisCommunicationsRepairParser::onFramesPerSecond(uint8_t instance_id, int frames_per_sec)
{
    if ((frames_per_sec < 1) || (frames_per_sec > 30))
    {
        throw bdf::invalid_data("Invalid frames per second!");
    }

    mSerializer.writeFramesPerSecond(instance_id, frames_per_sec);
}

void cAxisCommunicationsRepairParser::onImageSize(uint8_t instance_id, int width, int height)
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

    mSerializer.writeImageSize(instance_id, width, height);
}

void cAxisCommunicationsRepairParser::onBitmap(uint8_t instance_id, const cBitmapBuffer& buffer)
{
    mSerializer.write(instance_id, buffer);
}

void cAxisCommunicationsRepairParser::onJPEG(uint8_t instance_id, const cJpegBuffer& buffer)
{
    mSerializer.write(instance_id, buffer);
}

void cAxisCommunicationsRepairParser::onMpegFrame(uint8_t instance_id, const cMpegFrameBuffer& buffer)
{
    mSerializer.write(instance_id, buffer);
}

void cAxisCommunicationsRepairParser::processActiveCameraId(cDataBuffer& buffer)
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

void cAxisCommunicationsRepairParser::processFramesPerSecond(cDataBuffer& buffer)
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

void cAxisCommunicationsRepairParser::processBitmap(cDataBuffer& buffer)
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

void cAxisCommunicationsRepairParser::processJPEG(cDataBuffer& buffer)
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

void cAxisCommunicationsRepairParser::processMpegFrame(cDataBuffer& buffer)
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

void cAxisCommunicationsRepairParser::processImageSize(cDataBuffer& buffer)
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





