
#include "DataRepair.hpp"
#include "BlockDataFileExceptions.hpp"

#include <QDir>
#include <QFileInfo>


//-----------------------------------------------------------------------------
cDataRepair::cDataRepair(int id, const QString& dataDir, const QString& repairedDir, QObject* parent)
    : QObject(parent), mId(id)
{
    mCurrentDataDirectory = dataDir;
    mRepairedDataDirectory = repairedDir;
}

cDataRepair::~cDataRepair()
{
    cBlockDataFileReader::close();
    mFileWriter.close();
}

//-----------------------------------------------------------------------------
bool cDataRepair::open(const std::string& file_name)
{
    mCurrentFileName = QString::fromStdString(file_name);
    QString filename = QFileInfo(mCurrentFileName).fileName();
    mRepairedFileName = mRepairedDataDirectory + filename;

    if (QDir().exists(mRepairedFileName))
        return false;

    std::string repairFilename = mRepairedFileName.toStdString();
    if (!mFileWriter.open(repairFilename))
    {
        return false;
    }

    return cBlockDataFileReader::open(file_name);
}

//-----------------------------------------------------------------------------
void cDataRepair::run()
{
    if (!cBlockDataFileReader::isOpen())
    {
        emit fileResults(mId, false, "File is not open!");
        return;
    }

    try
    {
        while (!eof())
        {
            if (fail())
            {
                emit fileResults(mId, false, "I/O Error: failbit is set.");
                cBlockDataFileReader::close();
                return;
            }

//            try
            {
                cBlockDataFileReader::processBlock();
            }
//            catch (const bdf::crc_error& e)
            {
                // CRC error are not necessarily corrupt files
//                emit statusMessage(e.what());
            }
        }
    }
    catch (const bdf::stream_error& e)
    {
        std::string msg = e.what();
        emit fileResults(mId, false, e.what());
    }
    catch (const bdf::unexpected_eof& e)
    {
        moveFileToRepaired(false);

        QString msg = "Unexpected EOF: ";
        msg += e.what();
        emit fileResults(mId, true, msg);
        return;
    }
    catch (const std::exception& e)
    {
        if (eof())
        {
            if (!moveFileToRepaired())
                emit fileResults(mId, false, "File size mismatch!");
            else
                emit fileResults(mId, true, QString());
        }
        else
        {
            emit fileResults(mId, false, e.what());
        }
        return;
    }

    if (!moveFileToRepaired())
        emit fileResults(mId, false, "File size mismatch!");
    else
        emit fileResults(mId, true, QString());
}

void cDataRepair::processBlock(const cBlockID& id)
{
    mFileWriter.writeBlock(id);
}

void cDataRepair::processBlock(const cBlockID& id, const std::byte* buf, std::size_t len)
{
    mFileWriter.writeBlock(id, buf, len);
}

//-----------------------------------------------------------------------------
bool cDataRepair::moveFileToRepaired(bool size_check)
{
    if (cBlockDataFileReader::isOpen())
        cBlockDataFileReader::close();

    if (mFileWriter.isOpen())
        mFileWriter.close();

    if (size_check)
    {
        auto src = QFileInfo(mCurrentFileName).size();
        auto dst = QFileInfo(mRepairedFileName).size();
        std::size_t diff = 0;
        if (dst > src)
            diff = dst - src;
        else
            diff = src - dst;

        // The 4096 is one hard drive data block.
        if (diff > 4096)
        {
            return false;
        }
    }

    if (!QDir().exists(mCurrentDataDirectory))
        return false;

    // Move the failed data file to a "recovered" directory
    // to signal that the file was fully repaired!
    {
        QString path = mCurrentDataDirectory;
        path += "/recovered";

        QDir recoveredDir(path);
        if (!recoveredDir.exists())
        {
            if (!QDir().mkdir(path))
                return false;
        }

        QString filename = QFileInfo(mCurrentFileName).fileName();
        QString newName = path + "/" + filename;

        QDir().rename(mCurrentFileName, newName);
    }

    // Move the fully repaired data file back into the main data
    // directory.
    {
        QDir path = mRepairedDataDirectory;
        path.cdUp();

        QString filename = QFileInfo(mRepairedFileName).fileName();
        auto fi = QFileInfo(path, filename);
        if (!fi.exists())
        {
            QString newName = fi.absoluteFilePath();
            if (!QDir().rename(mRepairedFileName, newName))
            {
                std::string fn1 = mRepairedFileName.toStdString();
                std::string fn2 = newName.toStdString();
            }
        }
    }

    return true;
}


