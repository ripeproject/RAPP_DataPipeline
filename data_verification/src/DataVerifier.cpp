
#include "DataVerifier.hpp"
#include "ParserExceptions.hpp"
#include "OusterVerificationParser.hpp"
#include "AxisCommunicationsVerificationParser.hpp"

#include <cbdf/BlockDataFileExceptions.hpp>

#include <memory>
#include <string>
#include <iostream>


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
cDataVerifier::cDataVerifier(std::filesystem::path failed_dir)
{
    mFailedDirectory = failed_dir;
}

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
bool cDataVerifier::open(std::filesystem::directory_entry file_to_check)
{
    if (mFileReader.isOpen())
        mFileReader.close();
        
    mCurrentFile = file_to_check;
    mFileReader.open(file_to_check.path().string());

    return mFileReader.isOpen();
}

//-----------------------------------------------------------------------------
void cDataVerifier::process_file(std::filesystem::directory_entry file_to_check)
{
    if (open(file_to_check))
    {
        std::cout << "Processing " << file_to_check << "..." << std::endl;

        run();
    }
}

//-----------------------------------------------------------------------------
void cDataVerifier::run()
{
    auto ouster = std::make_unique<cOusterVerificationParser>();
    auto axis = std::make_unique<cAxisCommunicationsVerificationParser>();

    mFileReader.attach(ouster.get());
    mFileReader.attach(axis.get());

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


