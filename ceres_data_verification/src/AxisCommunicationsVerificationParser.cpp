
#if 0

#include "AxisCommunicationsParser.hpp"
#include "AxisDataIdentifiers.hpp"
#include "BlockDataFile.hpp"
#include "../Sensors/RGB/AxisCommunications/AxisCommunicationsUtils.hpp"

#include <QBitmap>
#include <QImage>

#include <cassert>

using namespace axis;

cAxisCommunicationsParser::cAxisCommunicationsParser()
:
    cBlockParser(),
    mImageData(),
    mImageBuffer(&mImageData)
{
    mImageBuffer.open(QIODevice::ReadWrite);
    mImageReader.setDevice(&mImageBuffer);

    auto list = QImageReader::supportedImageFormats();
}

cBlockID& cAxisCommunicationsParser::blockID()
{
	return mBlockID;
}

void cAxisCommunicationsParser::processData(BLOCK_MAJOR_VERSION_t major_version,
    BLOCK_MINOR_VERSION_t minor_version,
    BLOCK_DATA_ID_t data_id,
    cDataBuffer& buffer)
{
    mBlockID.setVersion(major_version, minor_version);
    mBlockID.dataID(static_cast<axis::DataID>(data_id));

    switch (static_cast<axis::DataID>(data_id))
    {
    case DataID::CAMERA_ID:
        processActiveCameraId(buffer);
        break;
    case DataID::FRAMES_PER_SECOND:
        processFramesPerSecond(buffer);
        break;
    case DataID::BITMAP:
        processBitmap(buffer);
        break;
    case DataID::JPEG:
        processJPEG(buffer);
        break;
    case DataID::MPEG_FRAME:
        processMpegFrame(buffer);
        break;
    case DataID::RESOLUTION:
        processImageSize(buffer);
        break;
    case DataID::TIMESTAMP:
        //(buffer);
        break;
    }
}

void cAxisCommunicationsParser::processActiveCameraId(cDataBuffer& buffer)
{
    int id = buffer.get<int32_t>();

    onActiveCameraId(id);
}

void cAxisCommunicationsParser::processFramesPerSecond(cDataBuffer& buffer)
{
    int frames_per_sec = buffer.get<int32_t>();

    onFramesPerSecond(frames_per_sec);
}

void cAxisCommunicationsParser::processBitmap(cDataBuffer& buffer)
{
    QBitmap bitmap;
    mImageReader.setFormat("bmp");

    uint32_t size;
    buffer >> size;

}

void cAxisCommunicationsParser::processJPEG(cDataBuffer& buffer)
{
    mImageReader.setFormat("jpeg");
}

void cAxisCommunicationsParser::processMpegFrame(cDataBuffer& buffer)
{
    mImageReader.setDevice(&mImageBuffer);
    mImageReader.setFormat("jpeg");

    uint32_t size;
    buffer >> size;

    mImageData.resize(size+16);
    buffer.read(mImageData.data(), size);

    mImageBuffer.seek(0);

    auto can_read = mImageReader.canRead();

    QImage image = mImageReader.read();
    auto ok = image.isNull();

    onMpegFrame(image);

    mImageData.clear();
    mImageBuffer.seek(0);
}

void cAxisCommunicationsParser::processImageSize(cDataBuffer& buffer)
{
    uint16_t width = 0;
    uint16_t height = 0;

    buffer >> width;
    buffer >> height;

    onImageSize(width, height);
}


#if 0

void cAxisCommunicationsSerializer::writeBitmap(const QBitmap& in)
{
    assert(mpDataFile);

    mImageWriter.setFormat("bmp");
    mImageWriter.write(in.toImage());

    mBlockID.dataID(DataID::JPEG);

    mDataBuffer.clear();
    mDataBuffer << mImageData.size();
    mDataBuffer.write(mImageData.constData(), mImageData.size());

    mpDataFile->writeBlock(mBlockID, mDataBuffer.data(), mDataBuffer.size());
    mImageData.clear();
}

void cAxisCommunicationsSerializer::writeJPEG(const QImage& in)
{
    assert(mpDataFile);

    mImageWriter.setFormat("jpeg");
    mImageWriter.write(in);

    mBlockID.dataID(DataID::JPEG);

    mDataBuffer.clear();
    mDataBuffer << mImageData.size();
    mDataBuffer.write(mImageData.constData(), mImageData.size());

    mpDataFile->writeBlock(mBlockID, mDataBuffer.data(), mDataBuffer.size());
    mImageData.clear();
}

void cAxisCommunicationsSerializer::write(const axis::sImageSize_t& in)
{
    assert(mpDataFile);

    mBlockID.dataID(DataID::RESOLUTION);

    mDataBuffer.clear();
    mDataBuffer << in.width;
    mDataBuffer << in.height;

    mpDataFile->writeBlock(mBlockID, mDataBuffer.data(), mDataBuffer.size());
}
#endif

#endif
