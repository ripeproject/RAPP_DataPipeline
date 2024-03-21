
#include "FileProcessor.hpp"

#include "ProcessingInfoLoader.hpp"
#include "PointCloudLoader.hpp"

#include "PlotBoundaries.hpp"
#include "StringUtils.hpp"
#include "ExportUtils.hpp"

#include "ProcessingInfoSerializer.hpp"
#include "PlotInfoSerializer.hpp"

#include <cbdf/ExperimentSerializer.hpp>
#include <cbdf/BlockDataFileExceptions.hpp>
#include <cbdf/ExperimentInfoLoader.hpp>

#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <numbers>
#include <algorithm>


extern void console_message(const std::string& msg);
extern void new_file_progress(const int id, std::string filename);
extern void update_prefix_progress(const int id, std::string prefix, const int progress_pct);
extern void update_progress(const int id, const int progress_pct);
extern void complete_file_progress(const int id);


namespace
{
    void save_plot_to_pointcloud(const cRappPointCloud& in, cPointCloud& out)
    {
        assert(in.size() == out.size());

        double x_min_m = in.minX_mm() * nConstants::MM_TO_M;
        double x_max_m = in.maxX_mm() * nConstants::MM_TO_M;
        double y_min_m = in.minY_mm() * nConstants::MM_TO_M;
        double y_max_m = in.maxY_mm() * nConstants::MM_TO_M;
        double z_min_m = in.minZ_mm() * nConstants::MM_TO_M;
        double z_max_m = in.maxZ_mm() * nConstants::MM_TO_M;

        out.setExtents(x_min_m, x_max_m, y_min_m, y_max_m, z_min_m, z_max_m);

        auto n = out.size();
        for (std::size_t i = 0; i < n; ++i)
        {
            auto& p1 = out[i];
            auto p2 = in[i];

            p1.X_m = p2.x_mm * nConstants::MM_TO_M;
            p1.Y_m = p2.y_mm * nConstants::MM_TO_M;
            p1.Z_m = p2.z_mm * nConstants::MM_TO_M;

            p1.range_mm = p2.range_mm;
            p1.signal = p2.signal;
            p1.reflectivity = p2.reflectivity;
            p1.nir = p2.nir;
        }
    }
}


cFileProcessor::cFileProcessor(int id, std::filesystem::directory_entry in,
                                std::filesystem::path out) 
:
    mID(id)
{
    mInputFile = in;
    mOutputFile = out;

    mProcessingInfo = std::make_shared<cProcessingInfo>();
    mExpInfo = std::make_shared<cExperimentInfo>();
}

cFileProcessor::~cFileProcessor()
{
    mFileReader.close();

    for (auto plot : mPlots)
    {
        delete plot;
    }

    mPlots.clear();
}

bool cFileProcessor::open()
{
    if (std::filesystem::exists(mOutputFile))
    {
        return false;
    }

    mFileReader.open(mInputFile.string());

    mFileSize = mFileReader.file_size();

    return mFileReader.isOpen();
}

void cFileProcessor::savePlotsInSingleFile(bool singleFile)
{
    mSavePlotsInSingleFile = singleFile;
}

void cFileProcessor::savePlyFiles(bool savePlys)
{
    mSavePlyFiles = savePlys;
}

void cFileProcessor::plyUseBinaryFormat(bool binaryFormat)
{
    mPlyUseBinaryFormat = binaryFormat;
}

void cFileProcessor::setPlotInfo(std::shared_ptr<cPlotBoundaries> plot_info)
{
    mPlotInfo = plot_info;
}

void cFileProcessor::process_file()
{
    if (open())
    {
        new_file_progress(mID, mInputFile.string());

        run();
    }
}

void cFileProcessor::run()
{
    update_prefix_progress(mID, "Loading...              ", 0);
    if (!loadFileData())
	{
        return;
	}

    update_prefix_progress(mID, "Splitting in Plots...    ", 0);
    doPlotSplit();

    update_prefix_progress(mID, "Saving...                ", 0);
    if (mSavePlotsInSingleFile)
        savePlotFile();
    else
        savePlotFiles();

    if (mSavePlyFiles)
    {
        update_prefix_progress(mID, "Saving PLY files...      ", 0);
        savePlyFiles();
    }

    update_prefix_progress(mID, "       Complete          ", 100);
    complete_file_progress(mID);
}

bool cFileProcessor::loadFileData()
{
    if (!mFileReader.isOpen())
    {
        throw std::logic_error("No file is open for reading.");
    }

    int last_file_pos_pct = 0;

    std::unique_ptr<cProcessingInfoLoader> pProcessingInfo  = std::make_unique<cProcessingInfoLoader>(mProcessingInfo);
    std::unique_ptr<cExperimentInfoLoader> pExpInfo         = std::make_unique<cExperimentInfoLoader>(mExpInfo);
    std::unique_ptr<cPointCloudLoader> pPcInfo              = std::make_unique<cPointCloudLoader>(mPointClouds);

    mFileReader.attach(pProcessingInfo.get());
    mFileReader.attach(pExpInfo.get());
    mFileReader.attach(pPcInfo.get());

    try
    {
        while (!mFileReader.eof())
        {
            if (mFileReader.fail())
            {
                mFileReader.close();

                return true;
            }

            mFileReader.processBlock();

            auto file_pos = static_cast<double>(mFileReader.filePosition());
            mFilePos = 100.0 * (file_pos / mFileSize);

            if (static_cast<int>(mFilePos) != last_file_pos_pct)
            {
                update_progress(mID, static_cast<int>(mFilePos));
                last_file_pos_pct = static_cast<int>(mFilePos);
            }
        }
    }
    catch (const bdf::stream_error& e)
    {
        std::string msg = "Stream Error: ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const bdf::crc_error& e)
    {
        std::string msg = "CRC Error: ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const bdf::unexpected_eof& e)
    {
        std::string msg = "Unexpected EOF: ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const std::exception& e)
    {
        std::string msg = "Unknown Exception: ";
        msg += e.what();
        console_message(msg);

        return false;
    }

    mFileReader.close();

    return true;
}

//-----------------------------------------------------------------------------
void cFileProcessor::doPlotSplit()
{
    auto n = mPointClouds.size();
    int i = 0;

    for (auto& info : mPointClouds)
    {
        update_progress(mID, static_cast<int>((100.0 * i++) / n));

        auto pointCloud = info.pointCloud();
        pointCloud.trim_outside(mPlotInfo->getBoundingBox());

        auto info_plots = mPlotInfo->getPlots();

        for (auto plot_info : info_plots)
        {
            std::unique_ptr<cRappPointCloud> plotPointCloud = std::make_unique<cRappPointCloud>(pointCloud);

            plotPointCloud->trim_outside(plot_info->getBoundingBox());

            plotPointCloud->trim_outside(plot_info->getPlotBounds());

            cRappPlot* plot = new cRappPlot(plot_info->getPlotNumber());

            plot->setPlotName(plot_info->getPlotName());
            plot->setEvent(plot_info->getEvent());
            plot->setDescription(plot_info->getDescription());
            plot->setPointCloud(*plotPointCloud);

            mPlots.push_back(plot);
        }
    }
}

//-----------------------------------------------------------------------------
void cFileProcessor::savePlotFile()
{
    auto n = mPlots.size();
    int i = 0;


    std::string filename = mOutputFile.string();
    filename = nStringUtils::addProcessedTimestamp(filename);
    filename += ".ceres";

    cBlockDataFileWriter fileWriter;

    cExperimentSerializer       experimentSerializer;
    cProcessingInfoSerializer   processInfoSerializer;
    cPlotInfoSerializer 	    plotInfoSerializer;

    experimentSerializer.setBufferCapacity(4096 * 1024);
    processInfoSerializer.setBufferCapacity(4096);

    experimentSerializer.attach(&fileWriter);
    processInfoSerializer.attach(&fileWriter);
    plotInfoSerializer.attach(&fileWriter);

    fileWriter.open(filename);
    if (!fileWriter.isOpen())
        return;

    writeProcessingInfo(processInfoSerializer);

    writeExperimentInfo(experimentSerializer);

    for (auto plot : mPlots)
    {
        plotInfoSerializer.setBufferCapacity(plot->data().size() * sizeof(cRappPlot::value_type));

        update_progress(mID, static_cast<int>((100.0 * i++) / n));
        plotInfoSerializer.writeID(plot->id());

        if (!plot->name().empty())
            plotInfoSerializer.writeName(plot->name());

        if (!plot->event().empty())
            plotInfoSerializer.writeEvent(plot->event());

        if (!plot->description().empty())
            plotInfoSerializer.writeDescription(plot->description());

        plotInfoSerializer.writeDescription(plot->description());

        cPointCloud point_cloud;
        point_cloud.resize(plot->pointCloud().size());
        save_plot_to_pointcloud(plot->pointCloud(), point_cloud);

        plotInfoSerializer.writeDimensions(point_cloud.minX_m(), point_cloud.maxX_m(),
            point_cloud.minY_m(), point_cloud.maxY_m(), point_cloud.minZ_m(), point_cloud.maxZ_m());

        plotInfoSerializer.write(point_cloud);
    }

    fileWriter.close();
}

//-----------------------------------------------------------------------------
void cFileProcessor::savePlotFiles()
{
    auto n = mPlots.size();
    int i = 0;

    for (auto plot : mPlots)
    {
        update_progress(mID, static_cast<int>((100.0 * i++) / n));

        std::string filename = mOutputFile.string();
        filename += "_Plot";
        filename += std::to_string(plot->id());
        filename = nStringUtils::addProcessedTimestamp(filename);
        filename += ".ceres";

        cBlockDataFileWriter fileWriter;

        cExperimentSerializer       experimentSerializer;
        cProcessingInfoSerializer   processInfoSerializer;
        cPlotInfoSerializer 	    plotInfoSerializer;

        experimentSerializer.setBufferCapacity(4096 * 1024);
        processInfoSerializer.setBufferCapacity(4096);
        plotInfoSerializer.setBufferCapacity(plot->data().size() * sizeof(cRappPlot::value_type));

        experimentSerializer.attach(&fileWriter);
        processInfoSerializer.attach(&fileWriter);
        plotInfoSerializer.attach(&fileWriter);

        fileWriter.open(filename);
        if (!fileWriter.isOpen())
            continue;

        writeProcessingInfo(processInfoSerializer);

        writeExperimentInfo(experimentSerializer);

        plotInfoSerializer.writeID(plot->id());

        if (!plot->name().empty())
            plotInfoSerializer.writeName(plot->name());

        if (!plot->event().empty())
            plotInfoSerializer.writeEvent(plot->event());

        if (!plot->description().empty())
            plotInfoSerializer.writeDescription(plot->description());

        plotInfoSerializer.writeDescription(plot->description());

        cPointCloud point_cloud;
        point_cloud.resize(plot->pointCloud().size());
        save_plot_to_pointcloud(plot->pointCloud(), point_cloud);

        plotInfoSerializer.writeDimensions(point_cloud.minX_m(), point_cloud.maxX_m(), 
            point_cloud.minY_m(), point_cloud.maxY_m(), point_cloud.minZ_m(), point_cloud.maxZ_m());

        plotInfoSerializer.write(point_cloud);

        fileWriter.close();
    }
}

//-----------------------------------------------------------------------------
void cFileProcessor::savePlyFiles()
{
    auto n = mPlots.size();
    int i = 0;

    for (auto plot : mPlots)
    {
        update_progress(mID, static_cast<int>((100.0 * i++) / n));

        std::string filename = mOutputFile.string();
        filename += "_Plot";
        filename += std::to_string(plot->id());

        if (!plot->name().empty())
        {
            filename += "_";
            filename += plot->name();
        }

        filename += ".ply";
        exportPointcloud2Ply(filename, plot->data(), mPlyUseBinaryFormat);
    }
}

//-----------------------------------------------------------------------------
void cFileProcessor::writeProcessingInfo(cProcessingInfoSerializer& serializer)
{
    serializer.write("plot_split", processing_info::ePROCESSING_TYPE::PLOT_SPLITTING);

    for (auto it = mProcessingInfo->begin(); it != mProcessingInfo->end(); ++it)
    {
        serializer.write(*it);
    }
}

//-----------------------------------------------------------------------------
void cFileProcessor::writeExperimentInfo(cExperimentSerializer& serializer)
{
    serializer.writeBeginHeader();

    if (!mExpInfo->title().empty())
        serializer.writeTitle(mExpInfo->title());

    if (!mExpInfo->experimentDoc().empty())
        serializer.writeExperimentDoc(mExpInfo->experimentDoc());

    if (!mExpInfo->comments().empty())
        serializer.writeComments(mExpInfo->comments());

    if (!mExpInfo->principalInvestigator().empty())
        serializer.writePrincipalInvestigator(mExpInfo->principalInvestigator());

    if (!mExpInfo->researchers().empty())
        serializer.writeResearchers(mExpInfo->researchers());

    if (!mExpInfo->species().empty())
        serializer.writeSpecies(mExpInfo->species());

    if (!mExpInfo->cultivar().empty())
        serializer.writeCultivar(mExpInfo->cultivar());

    if (!mExpInfo->construct().empty())
        serializer.writeConstructName(mExpInfo->construct());

    if (!mExpInfo->eventNumbers().empty())
        serializer.writeEventNumbers(mExpInfo->eventNumbers());

    if (!mExpInfo->treatments().empty())
        serializer.writeTreatments(mExpInfo->treatments());

    if (!mExpInfo->fieldDesign().empty())
        serializer.writeFieldDesign(mExpInfo->fieldDesign());

    if (!mExpInfo->permit().empty())
        serializer.writePermitInfo(mExpInfo->permit());

    if (mExpInfo->plantingDate().has_value())
    {
        nExpTypes::sDateDoy_t date = mExpInfo->plantingDate().value();
        serializer.writePlantingDate(date.year, date.month, date.day, date.doy);
    }

    if (mExpInfo->harvestDate().has_value())
    {
        nExpTypes::sDateDoy_t date = mExpInfo->harvestDate().value();
        serializer.writeHarvestDate(date.year, date.month, date.day, date.doy);
    }

    if (mExpInfo->fileDate().has_value())
    {
        nExpTypes::sDate_t date = mExpInfo->fileDate().value();
        serializer.writeFileDate(date.year, date.month, date.day);
    }

    if (mExpInfo->fileTime().has_value())
    {
        nExpTypes::sTime_t time = mExpInfo->fileTime().value();
        serializer.writeFileTime(time.hour, time.minute, time.seconds);
    }

    if (mExpInfo->dayOfYear().has_value())
        serializer.writeDayOfYear(mExpInfo->dayOfYear().value());

    serializer.writeEndOfHeader();
}

