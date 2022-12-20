
#include "DataVerifier.hpp"
#include "ParserExceptions.hpp"
#include "OusterVerificationParser.hpp"

#include <CBDF/BlockDataFileExceptions.hpp>

#include <memory>

namespace
{
    void create_directory(std::filesystem::path failed_dir)
    {
        if (!std::filesystem::exists(failed_dir))
        {
            std::filesystem::create_directory(failed_dir);
        }
    }
}

//-----------------------------------------------------------------------------
cDataVerifier::cDataVerifier(std::filesystem::directory_entry file_to_check,
    std::filesystem::path failed_dir)
{
    mFailedDirectory = failed_dir;
    mCurrentFile = file_to_check;
    mFileReader.open(file_to_check.path().string());

    if (!mFileReader.isOpen())
    {
        throw std::logic_error(mCurrentFile.string());
    }
}

cDataVerifier::~cDataVerifier()
{
    mFileReader.close();
}

//-----------------------------------------------------------------------------
void cDataVerifier::run()
{
    auto ouster = std::make_unique<cOusterVerificationParser>();

    mFileReader.attach(ouster.get());
//    mFileReader.attach(static_cast<cAxisCommunicationsParser*>(this));

    try
    {
        while (!mFileReader.eof())
        {
            if (mFileReader.fail())
            {
                mFileReader.close();
                return;
            }

            mFileReader.processBlock();
        }
    }
    catch (const bdf::stream_error& e)
    {
        mFileReader.close();
        std::string msg = e.what();

        moveFileToFailed();
        return;
    }
    catch (const std::exception& e)
    {
        if (!mFileReader.eof())
        {
            moveFileToFailed();
        }
        return;
    }

    mFileReader.close();
}

//-----------------------------------------------------------------------------
void cDataVerifier::moveFileToFailed()
{
    if (mFileReader.isOpen())
        mFileReader.close();

    ::create_directory(mFailedDirectory);

    std::filesystem::path dest = mFailedDirectory / mCurrentFile.filename();
    std::filesystem::rename(mCurrentFile, dest);
}

//-----------------------------------------------------------------------------

#if 0
void cDataVerifier::onActiveCameraId(int id)
{
    if ((id < 1) || (id > 4))
    {
        throw bdf::parse_error("Invalid active camera id!");
    }
}

void cDataVerifier::onFramesPerSecond(int frames_per_sec)
{
    if ((frames_per_sec < 1) || (frames_per_sec > 30))
    {
        throw bdf::parse_error("Invalid frames per second!");
    }
}

void cDataVerifier::onBitmap(const QBitmap& in)
{}

void cDataVerifier::onJPEG(const QImage& image)
{}

void cDataVerifier::onMpegFrame(const QImage& image)
{}

void cDataVerifier::onImageSize(int width, int height)
{
    bool validWidth = (width == 480) || (width == 640) || (width == 800) ||
        (width == 854) || (width == 1024) || (width == 1280) || (width == 1920);

    bool validHeight = (height == 270) || (height == 300) || (height == 360)
        || (height == 400) || (height == 450) || (height == 480) || (height == 500)
        || (height == 600) || (height == 640) || (height == 720) || (height == 768)
        || (height == 1080);

    if (!validWidth || !validHeight)
    {
        throw bdf::parse_error("Invalid image size!");
    }
}
#endif

