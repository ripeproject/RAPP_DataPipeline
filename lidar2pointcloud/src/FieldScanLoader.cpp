
#include "FieldScanLoader.hpp"

#include "FieldScanDataModel.hpp"

#include "OusterInfoLoader.hpp"
#include "ProcessingInfoLoader.hpp"

#include <cbdf/BlockDataFile.hpp>
#include <cbdf/BlockDataFileExceptions.hpp>
#include <cbdf/ExperimentInfoLoader.hpp>
#include <cbdf/SsnxInfoLoader.hpp>
#include <cbdf/SpiderCamInfoLoader.hpp>

#include <iostream>
#include <filesystem>


extern void update_progress(const int id, const int progress_pct);


cFieldScanLoader::cFieldScanLoader(int id, cFieldScanDataModel& model)
    :
        mID(id), mModel(model)
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

bool cFieldScanLoader::run()
{
    cBlockDataFileReader fileReader;

    if (!fileReader.open(mFilename))
    {
        return false;
    }

    auto processing = std::make_unique<cProcessingInfoLoader>(mModel.getProcessingInfo());
    auto info = std::make_unique<cExperimentInfoLoader>(mModel.getExperimentInfo());
    auto ctrl = std::make_unique<cSpiderCamInfoLoader>(mModel.getSpiderCamInfo());
    auto ssnx = std::make_unique<cSsnxInfoLoader>(mModel.getSsnxInfo());
    auto ouster = std::make_unique<cOusterInfoLoader>(mModel.getOusterInfo());

    fileReader.attach(processing.get());
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
                return true;
            }

            fileReader.processBlock();

            test_pos = fileReader.filePosition();
            double file_pos = static_cast<double>(test_pos);

            int progress_pct = static_cast<int>(100.0 * (file_pos / file_size));
            update_progress(mID, progress_pct);
        }
    }
    catch (const bdf::stream_error& e)
    {
        return false;
    }
    catch (const bdf::crc_error& e)
    {
        return false;
    }
    catch (const bdf::unexpected_eof& e)
    {
        return false;
    }
    catch (const std::exception& e)
    {
        return false;
    }

    update_progress(mID, 100);

    return true;
}


