
#include "FileProcessor.hpp"

#include "ProcessingInfoLoader.hpp"
#include "PointCloudLoader.hpp"

#include "PlotConfigScan.hpp"

#include "PlotSplitUtils.hpp"

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

    void save_plot_to_pointcloud(const cRappPointCloud& in, cPointCloud_FrameId& out)
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

            p1.frameID = p2.frameID;
        }
    }

    void save_plot_to_pointcloud(const cRappPointCloud& in, cPointCloud_SensorInfo& out)
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

            p1.frameID  = p2.frameID;
            p1.chnNum   = p2.frameID;
            p1.pixelNum = p2.pixelNum;
        }
    }
}


cFileProcessor::cFileProcessor(int id, std::filesystem::directory_entry in,
                                std::filesystem::path out, const cPlotConfigScan& plot_info)
:
    mID(id), mPlotInfo(plot_info)
{
    mInputFile = in;
    mOutputFile = out;

    mProcessingInfo = std::make_shared<cProcessingInfo>();
    mExpInfo        = std::make_shared<cExperimentInfo>();
    mPointCloudInfo = std::make_shared<cPointCloudInfo>();
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

void cFileProcessor::enableSavingFrameIDs(bool enableFrameIDs)
{
    mEnableFrameIDs = enableFrameIDs;
}

void cFileProcessor::enableSavingPixelInfo(bool enablePixelInfo)
{
    mEnablePixelInfo = enablePixelInfo;
}

/*
void cFileProcessor::setPlotInfo(std::shared_ptr<cPlotBoundaries> plot_info)
{
    mPlotInfo = plot_info;
}
*/

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
    std::unique_ptr<cPointCloudLoader> pPcInfo              = std::make_unique<cPointCloudLoader>(mPointCloudInfo);

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
    auto n = mPointCloudInfo->numPointClouds() * mPlotInfo.size();
    int i = 0;

    for (auto pointCloud : mPointCloudInfo->getPointClouds())
    {
        for (const auto& plotInfo : mPlotInfo)
        {
            update_progress(mID, static_cast<int>((100.0 * i++) / n));

            const auto& bounds = plotInfo.getBounds();
            bool hasSubPlot = bounds.hasSubPlots();

            const auto& method = plotInfo.getIsolationMethod();

            switch (method.getMethod())
            {
            case ePlotIsolationMethod::NONE:
            {
                {
                    auto plotPointCloud = isolate_basic(pointCloud, bounds.getBoundingBox());

                    if (plotPointCloud.empty())
                    {
                        std::string msg = "Point cloud \"";

                        if (pointCloud.name().empty())
                        {
                            msg += mExpInfo->title();
                        }
                        else
                        {
                            msg += pointCloud.name();
                        }
                        msg += "\" is empty!";
                        console_message(msg);
                        continue;
                    }

                    cRappPlot* plot = new cRappPlot(plotInfo.getPlotNumber());

                    fillPlotInformation(plot, plotInfo);

                    plot->setPointCloud(plotPointCloud);

                    mPlots.push_back(plot);
                }

                if (hasSubPlot)
                {
                    auto plotPointClouds = isolate_basic(pointCloud, bounds.getBoundingBox(), 
                        bounds.getNumOfSubPlots(), bounds.getSubPlotOrientation(), 
                        method.getPlotWidth_mm(), method.getPlotLength_mm());

                    int subPlotId = 0;
                    for (const auto& plotPointCloud : plotPointClouds)
                    {
                        if (plotPointCloud.empty())
                        {
                            std::string msg = "Point cloud \"";
                            msg += pointCloud.name();
                            msg += "\" is empty!";
                            console_message(msg);
                            continue;
                        }

                        cRappPlot* plot = new cRappPlot(plotInfo.getPlotNumber(), ++subPlotId);

                        fillPlotInformation(plot, plotInfo);

                        plot->setPointCloud(plotPointCloud);

                        mPlots.push_back(plot);

                    }

                }
                break;
            }
            case ePlotIsolationMethod::CENTER_OF_PLOT:
            {
                auto plotPointCloud = isolate_center_of_plot(pointCloud, bounds.getBoundingBox(),
                    method.getPlotWidth_mm(), method.getPlotLength_mm());

                if (plotPointCloud.empty())
                {
                    std::string msg = "Point cloud \"";
                    msg += pointCloud.name();
                    msg += "\" is empty!";
                    console_message(msg);
                    continue;
                }

                cRappPlot* plot = new cRappPlot(plotInfo.getPlotNumber());

                fillPlotInformation(plot, plotInfo);

                plot->setPointCloud(plotPointCloud);

                mPlots.push_back(plot);
                break;
            }
            case ePlotIsolationMethod::CENTER_OF_HEIGHT:
            {
                if (hasSubPlot)
                {
                    {
                        auto plotPointCloud = isolate_basic(pointCloud, bounds.getBoundingBox());

                        if (plotPointCloud.empty())
                        {
                            std::string msg = "Point cloud \"";
                            msg += pointCloud.name();
                            msg += "\" is empty!";
                            console_message(msg);
                            continue;
                        }

                        cRappPlot* plot = new cRappPlot(plotInfo.getPlotNumber());

                        fillPlotInformation(plot, plotInfo);

                        plot->setPointCloud(plotPointCloud);

                        mPlots.push_back(plot);
                    }

                    auto plotPointClouds = isolate_center_of_height(pointCloud, bounds.getBoundingBox(),
                        bounds.getNumOfSubPlots(), bounds.getSubPlotOrientation(),
                        method.getPlotWidth_mm(), method.getPlotLength_mm(), method.getHeightThreshold_pct());

                    int subPlotId = 0;
                    for (const auto& plotPointCloud : plotPointClouds)
                    {
                        if (plotPointCloud.empty())
                        {
                            std::string msg = "Point cloud \"";
                            msg += pointCloud.name();
                            msg += "\" is empty!";
                            console_message(msg);
                            continue;
                        }

                        cRappPlot* plot = new cRappPlot(plotInfo.getPlotNumber(), ++subPlotId);

                        fillPlotInformation(plot, plotInfo);

                        plot->setPointCloud(plotPointCloud);

                        mPlots.push_back(plot);

                    }
                }
                else
                {
                    auto plotPointCloud = isolate_center_of_height(pointCloud, bounds.getBoundingBox(),
                        method.getPlotWidth_mm(), method.getPlotLength_mm(), method.getHeightThreshold_pct());

                    if (plotPointCloud.empty())
                    {
                        std::string msg = "Point cloud \"";
                        msg += pointCloud.name();
                        msg += "\" is empty!";
                        console_message(msg);
                        continue;
                    }

                    cRappPlot* plot = new cRappPlot(plotInfo.getPlotNumber());

                    fillPlotInformation(plot, plotInfo);

                    plot->setPointCloud(plotPointCloud);

                    mPlots.push_back(plot);
                }
                break;
            }
            }
        }
    }
}

//-----------------------------------------------------------------------------
void cFileProcessor::fillPlotInformation(cRappPlot* plot, const cPlotConfigPlotInfo& info)
{
    if (!plot) return;

    plot->setPlotName(info.getPlotName());
    plot->setEvent(info.getEvent());
    plot->setDescription(info.getDescription());
    plot->setSpecies(info.getSpecies());
    plot->setCultivar(info.getCultivar());
    plot->setConstructName(info.getConstructName());
    plot->setPotLabel(info.getPotLabel());
    plot->setSeedGeneration(info.getSeedGeneration());
    plot->setCopyNumber(info.getCopyNumber());
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
        std::size_t buffersize = std::max<std::size_t>(plot->data().size() * sizeof(cRappPlot::value_type), 4096ULL);
        plotInfoSerializer.setBufferCapacity(buffersize);

        update_progress(mID, static_cast<int>((100.0 * i++) / n));

        if (plot->subPlotId() > 0)
            plotInfoSerializer.writeID(plot->id(), plot->subPlotId());
        else
            plotInfoSerializer.writeID(plot->id());

        if (!plot->name().empty())
            plotInfoSerializer.writeName(plot->name());

        if (!plot->event().empty())
            plotInfoSerializer.writeEvent(plot->event());

        if (!plot->description().empty())
            plotInfoSerializer.writeDescription(plot->description());

        if (!plot->species().empty())
            plotInfoSerializer.writeSpecies(plot->species());

        if (!plot->cultivar().empty())
            plotInfoSerializer.writeCultivar(plot->cultivar());

        if (!plot->constructName().empty())
            plotInfoSerializer.writeConstructName(plot->constructName());

        if (!plot->potLabel().empty())
            plotInfoSerializer.writePotLabel(plot->potLabel());

        if (!plot->seedGeneration().empty())
            plotInfoSerializer.writeSeedGeneration(plot->seedGeneration());

        if (!plot->copyNumber().empty())
            plotInfoSerializer.writeCopyNumber(plot->copyNumber());

        plotInfoSerializer.writeVegetationOnly(plot->vegetationOnly());


        if (plot->pointCloud().hasPixelInfo())
        {
            cPointCloud_SensorInfo point_cloud;
            point_cloud.resize(plot->pointCloud().size());
            save_plot_to_pointcloud(plot->pointCloud(), point_cloud);

            plotInfoSerializer.writeDimensions(point_cloud.minX_m(), point_cloud.maxX_m(),
                point_cloud.minY_m(), point_cloud.maxY_m(), point_cloud.minZ_m(), point_cloud.maxZ_m());

            plotInfoSerializer.write(point_cloud);
        }
        else if (plot->pointCloud().hasFrameIDs())
        {
            cPointCloud_FrameId point_cloud;
            point_cloud.resize(plot->pointCloud().size());
            save_plot_to_pointcloud(plot->pointCloud(), point_cloud);

            plotInfoSerializer.writeDimensions(point_cloud.minX_m(), point_cloud.maxX_m(),
                point_cloud.minY_m(), point_cloud.maxY_m(), point_cloud.minZ_m(), point_cloud.maxZ_m());

            plotInfoSerializer.write(point_cloud);
        }
        else
        {
            cPointCloud point_cloud;
            point_cloud.resize(plot->pointCloud().size());
            save_plot_to_pointcloud(plot->pointCloud(), point_cloud);

            plotInfoSerializer.writeDimensions(point_cloud.minX_m(), point_cloud.maxX_m(),
                point_cloud.minY_m(), point_cloud.maxY_m(), point_cloud.minZ_m(), point_cloud.maxZ_m());

            plotInfoSerializer.write(point_cloud);
        }
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

        if (plot->subPlotId() > 0)
            plotInfoSerializer.writeID(plot->id(), plot->subPlotId());
        else
            plotInfoSerializer.writeID(plot->id());

        if (!plot->name().empty())
            plotInfoSerializer.writeName(plot->name());

        if (!plot->event().empty())
            plotInfoSerializer.writeEvent(plot->event());

        if (!plot->description().empty())
            plotInfoSerializer.writeDescription(plot->description());

        if (!plot->species().empty())
            plotInfoSerializer.writeSpecies(plot->species());

        if (!plot->cultivar().empty())
            plotInfoSerializer.writeCultivar(plot->cultivar());

        if (!plot->constructName().empty())
            plotInfoSerializer.writeConstructName(plot->constructName());

        if (!plot->potLabel().empty())
            plotInfoSerializer.writePotLabel(plot->potLabel());

        if (!plot->seedGeneration().empty())
            plotInfoSerializer.writeSeedGeneration(plot->seedGeneration());

        if (!plot->copyNumber().empty())
            plotInfoSerializer.writeCopyNumber(plot->copyNumber());


        if (plot->pointCloud().hasPixelInfo())
        {
            cPointCloud_SensorInfo point_cloud;
            point_cloud.resize(plot->pointCloud().size());
            save_plot_to_pointcloud(plot->pointCloud(), point_cloud);

            plotInfoSerializer.writeDimensions(point_cloud.minX_m(), point_cloud.maxX_m(),
                point_cloud.minY_m(), point_cloud.maxY_m(), point_cloud.minZ_m(), point_cloud.maxZ_m());

            plotInfoSerializer.write(point_cloud);
        }
        else if (plot->pointCloud().hasFrameIDs())
        {
            cPointCloud_FrameId point_cloud;
            point_cloud.resize(plot->pointCloud().size());
            save_plot_to_pointcloud(plot->pointCloud(), point_cloud);

            plotInfoSerializer.writeDimensions(point_cloud.minX_m(), point_cloud.maxX_m(),
                point_cloud.minY_m(), point_cloud.maxY_m(), point_cloud.minZ_m(), point_cloud.maxZ_m());

            plotInfoSerializer.write(point_cloud);
        }
        else
        {
            cPointCloud point_cloud;
            point_cloud.resize(plot->pointCloud().size());
            save_plot_to_pointcloud(plot->pointCloud(), point_cloud);

            plotInfoSerializer.writeDimensions(point_cloud.minX_m(), point_cloud.maxX_m(),
                point_cloud.minY_m(), point_cloud.maxY_m(), point_cloud.minZ_m(), point_cloud.maxZ_m());

            plotInfoSerializer.write(point_cloud);
        }

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

        if (plot->subPlotId() > 0)
        {
            filename += ".";
            filename += std::to_string(plot->subPlotId());
        }

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

