
#include "DataVerifier.hpp"
#include "ParserExceptions.hpp"
#include "OusterVerificationParser.hpp"

#include <CBDF/BlockDataFileExceptions.hpp>

//#include <QDir>
//#include <QFileInfo>

#include <memory>


//-----------------------------------------------------------------------------
cDataVerifier::cDataVerifier(int id, const std::string& dataDir, void* parent)
    : mId(id)
{
    mCurrentDataDirectory = dataDir;
}

cDataVerifier::~cDataVerifier()
{}

//-----------------------------------------------------------------------------
bool cDataVerifier::open(const std::string& file_name)
{
    mCurrentFileName = file_name;
    return mFileReader.open(file_name);
}

//-----------------------------------------------------------------------------
void cDataVerifier::run()
{
    if (!mFileReader.isOpen())
    {
        fileResults(mId, false, "File is not open!");
        return;
    }

    auto ouster = std::make_unique<cOusterVerificationParser>();

    mFileReader.attach(ouster.get());
//    mFileReader.attach(static_cast<cAxisCommunicationsParser*>(this));

    try
    {
        while (!mFileReader.eof())
        {
            if (mFileReader.fail())
            {
                fileResults(mId, false, "I/O Error: failbit is set.");
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
        fileResults(mId, false, e.what());

        moveFileToFailed();
        return;
    }
    catch (const std::exception& e)
    {
        if (mFileReader.eof())
        {
            fileResults(mId, true, std::string());
        }
        else
        {
            fileResults(mId, false, e.what());
            moveFileToFailed();
        }
        mFileReader.close();
        return;
    }

    fileResults(mId, true, std::string());
    mFileReader.close();
}

//-----------------------------------------------------------------------------
void cDataVerifier::moveFileToFailed()
{
    if (mFileReader.isOpen())
        mFileReader.close();

/*
    if (!QDir().exists(mCurrentDataDirectory))
        return;

    QString path = mCurrentDataDirectory;
    path += "/failed";

    QDir failedDir(path);
    if (!failedDir.exists())
    {
        if (!QDir().mkdir(path))
            return;
    }

    QString filename = QFileInfo(mCurrentFileName).fileName();
    QString newName = path + "/" + filename;

    QDir().rename(mCurrentFileName, newName);
*/
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

