
#include "FieldScanLoader.hpp"

#include "FieldScanDataModel.hpp"

#include "OusterInfoLoader.hpp"

#include <cbdf/BlockDataFile.hpp>
#include <cbdf/BlockDataFileExceptions.hpp>
#include <cbdf/ExperimentInfoLoader.hpp>
#include <cbdf/SsnxInfoLoader.hpp>
#include <cbdf/SpiderCamInfoLoader.hpp>

#include <iostream>
#include <filesystem>


cFieldScanLoader::cFieldScanLoader(cFieldScanDataModel& model)
    :
        mModel(model)
{}

bool cFieldScanLoader::loadFile(const std::string& filename)
{
    if (!std::filesystem::exists(filename))
    {
        return false;
    }

    mFilename = filename;
    return true;
}

void cFieldScanLoader::run()
{
    cBlockDataFileReader fileReader;

    if (!fileReader.open(mFilename))
        return;

    auto info = std::make_unique<cExperimentInfoLoader>(mModel.getExperimentInfo());
    auto ctrl = std::make_unique<cSpiderCamInfoLoader>(mModel.getSpiderCamInfo());
    auto ssnx = std::make_unique<cSsnxInfoLoader>(mModel.getSsnxInfo());
    auto ouster = std::make_unique<cOusterInfoLoader>(mModel.getOusterInfo());

    fileReader.attach(info.get());
    fileReader.attach(ctrl.get());
    fileReader.attach(ssnx.get());
    fileReader.attach(ouster.get());

    auto file_size = fileReader.file_size();

    std::streampos test_pos;
    try
    {
        while (!fileReader.eof())
        {
            if (fileReader.fail())
            {
                fileReader.close();
                return;
            }

            fileReader.processBlock();

            test_pos = fileReader.filePosition();
            double file_pos = static_cast<double>(test_pos);

            file_pos = 100.0 * (file_pos / file_size);
//            emit progressUpdated(std::max(static_cast<int>(file_pos), 1));
        }
    }
    catch (const bdf::stream_error& e)
    {
//        emit progressUpdated(100);
//        emit loadingComplete();
        return;
    }
    catch (const bdf::crc_error& e)
    {
//        emit progressUpdated(100);
//        emit loadingComplete();
        return;
    }
    catch (const bdf::unexpected_eof& e)
    {
//        emit progressUpdated(100);
//        emit loadingComplete();
        return;
    }
    catch (const std::exception& e)
    {
//        emit progressUpdated(100);
//        emit loadingComplete();
        return;
    }

//    emit progressUpdated(100);
//    emit loadingComplete();
}


