
#include "FileProcessor.hpp"


#include "PlotDataConfigFile.hpp"

#include "PlotDataUtils.hpp"
#include "PlotSplitUtils.hpp"

#include "StringUtils.hpp"
#include "DateTimeUtils.hpp"

#include <cbdf/ProcessingInfoSerializer.hpp>
#include <cbdf/PlotInfoSerializer.hpp>
#include <cbdf/PlotInfoLoader.hpp>
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


cFileProcessor::cFileProcessor(int id, std::filesystem::directory_entry in,
    cPlotData& results, const cPlotDataConfigFile& plot_cfg)
:
    mID(id), mResults(results), mConfigInfo(plot_cfg)
{
    mInputFile = in;

    mExpInfo  = std::make_shared<cExperimentInfo>();
    mPlotInfo = std::make_shared<cPlotInfo>();
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
    mFileReader.open(mInputFile.string());

    mFileSize = mFileReader.file_size();

    return mFileReader.isOpen();
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
    fillGroupInfo();

    update_prefix_progress(mID, "Loading...              ", 0);
    if (!loadFileData())
	{
        return;
	}

    if (mPlotInfo->empty())
    {
        update_prefix_progress(mID, "    No plots found!      ", 100);
        complete_file_progress(mID);
        return;
    }

    mResults.setExperimentInfo(*mExpInfo);
    if (mConfigInfo.hasExperimentTitle())
        mResults.setExperimentTitle(mConfigInfo.getExperimentTitle());

    fillPlotInfo();

    update_prefix_progress(mID, "Computing Plot Heights...    ", 0);
    computePlotHeights();

    update_prefix_progress(mID, "Computing Plot Digital Biomass...    ", 0);
    computePlotBioMasses();

//    update_prefix_progress(mID, "Computing Plot LAI...    ", 0);
//    computePlotLAIs();

    if (mResults.hasGroupInfo())
    {
        update_prefix_progress(mID, "Computing Replicate Data...    ", 0);
        mResults.computeReplicateData();
    }
//    update_prefix_progress(mID, "Saving...                ", 0);
//    if (mSavePlotsInSingleFile)
//        savePlotFile();
//    else
//        savePlotFiles();

//    if (mSavePlyFiles)
//    {
//        update_prefix_progress(mID, "Saving PLY files...      ", 0);
//        savePlyFiles();
//    }

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

    std::unique_ptr<cExperimentInfoLoader> pExpInfo  = std::make_unique<cExperimentInfoLoader>(mExpInfo);
    std::unique_ptr<cPlotInfoLoader> pPlotInfo       = std::make_unique<cPlotInfoLoader>(mPlotInfo);

    mFileReader.attach(pExpInfo.get());
    mFileReader.attach(pPlotInfo.get());

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

void cFileProcessor::fillGroupInfo()
{
    auto groupby = mConfigInfo.getGroupBy();
    if (!groupby.empty())
    {
        for (auto type : groupby.getGroupByTypes())
        {
            switch (type)
            {
                case eGroupByTypes::EVENT:
                    mResults.useEvent(true);
                    break;
                case eGroupByTypes::SPECIES:
                    mResults.useSpecies(true);
                    break;
                case eGroupByTypes::CULTIVAR:
                    mResults.useCultivar(true);
                    break;
                case eGroupByTypes::TREATMENT:
                    mResults.useTreatment(true);
                    break;
                case eGroupByTypes::CONSTRUCT:
                    mResults.useConstructName(true);
                    break;
                case eGroupByTypes::POT_LABEL:
                    mResults.usePotLabel(true);
                    break;
                case eGroupByTypes::SEED_GENERATION:
                    mResults.useSeedGeneration(true);
                    break;
                case eGroupByTypes::COPY_NUMBER:
                    mResults.useCopyNumber(true);
                    break;
                case eGroupByTypes::LEAF_TYPE:
                    mResults.useLeafType(true);
                    break;
            }
        }
    }
}

void cFileProcessor::fillPlotInfo()
{
    int month = 0;
    int day = 0;
    int year = 0;

    auto date = mExpInfo->fileDate();

    if (date.has_value())
    {
        auto d = date.value();
        month = d.month;
        day = d.day;
        year = d.year;
    }

    if (mExpInfo->dayOfYear().has_value())
    {
        mDayOfYear = mExpInfo->dayOfYear().value();
    }
    else
    {
        mDayOfYear = nDateUtils::to_day_of_year(month, day, year);
    }

    mResults.addDate(month, day, year, mDayOfYear);
    auto n = mPlotInfo->size();

    for (std::size_t i = 0; i < n; ++i)
    {
        const auto& plot = (*mPlotInfo)[i];
        mResults.addPlotInfo(*plot);
    }

    mResults.splitIntoGroups();
}

void cFileProcessor::computePlotHeights()
{
    const auto& parameters = mConfigInfo.getHeightParameters();
    int groundLevelBound_mm = static_cast<int>(parameters.getGroundLevelBound_mm());
    double heightPercentile = parameters.getHeightPercentile();

    mResults.clearHeightMetaInfo();
    mResults.addHeightMetaInfo("algorithm: histogram");

    std::string info = "height percentile: ";
    info += std::to_string(heightPercentile);
    mResults.addHeightMetaInfo(info);

    bool hasFilters = parameters.hasFilters();
    const auto& filters = parameters.getFilters();

    if (hasFilters)
    {
        mResults.addHeightMetaInfo("");
        mResults.addHeightMetaInfo("Height Applied Filters:");

        for (auto* filter : filters)
        {
            mResults.addHeightMetaInfo(filter->info());
        }
    }

    auto n = mPlotInfo->size();

    for (std::size_t i = 0; i < n; ++i)
    {
        update_progress(mID, static_cast<int>((100.0 * i) / n));

        auto plot = *((*mPlotInfo)[i]);

        if (hasFilters)
        {
            for (auto* filter : filters)
            {
                filter->apply(plot);
            }
        }

        if (plot.vegetationOnly())
        {
            plot::trim_below_in_place(plot, groundLevelBound_mm);
        }
        else if (plot.groundLevel_mm().has_value())
        {
            auto groundShift_mm = plot.groundLevel_mm().value();

            plot::trim_below_in_place(plot, groundShift_mm);
            plot::translate(plot, 0, 0, -groundShift_mm);

            plot.clearGroundLevel_mm();

            plot::trim_below_in_place(plot, groundLevelBound_mm);
        }
        else
        {
            plot::trim_below_in_place(plot, groundLevelBound_mm);
            plot::translate(plot, 0, 0, -groundLevelBound_mm);
        }

        nPlotUtils::sHeightResults_t height = nPlotUtils::computePlotHeights(plot, heightPercentile, heightPercentile - 1, heightPercentile + 1);

        mResults.addPlotHeight(plot.id(), mDayOfYear, plot.size(), height.height_mm, height.lowerHeight_mm, height.upperHeight_mm);
    }
}

void cFileProcessor::computePlotBioMasses()
{
    const auto& parameters = mConfigInfo.getBiomassParameters();

    auto algorithm_type = parameters.getAlgorithmType();
    int groundLevelBound_mm = static_cast<int>(parameters.getGroundLevelBound_mm());
    double voxel_size_mm = parameters.getVoxelSize_mm();
    int min_bin_count = parameters.getMinBinCount();

    mResults.clearBiomassMetaInfo();
    switch (algorithm_type)
    {
    case eBiomassAlgorithmType::OCT_TREE:
        mResults.addBiomassMetaInfo("algorithm: voxelization (oct tree)");
        break;
    case eBiomassAlgorithmType::VOXEL_GRID:
        mResults.addBiomassMetaInfo("algorithm: voxelization (voxel grid)");
        break;
    case eBiomassAlgorithmType::CONVEX_HULL:
        mResults.addBiomassMetaInfo("algorithm: convex hull");
        break;
    default:
        mResults.addBiomassMetaInfo("algorithm: voxelization");
        break;
    }

    std::string info = "voxel size (mm): ";
    info += std::to_string(voxel_size_mm);
    mResults.addBiomassMetaInfo(info);

    switch (algorithm_type)
    {
    case eBiomassAlgorithmType::OCT_TREE:
        info = "min bin count: ";
        info += std::to_string(min_bin_count);
        mResults.addBiomassMetaInfo(info);
        break;
    case eBiomassAlgorithmType::VOXEL_GRID:
        if (min_bin_count > 0)
        {
            info = "min bin count: ";
            info += std::to_string(min_bin_count);
            mResults.addBiomassMetaInfo(info);
        }
        break;
    case eBiomassAlgorithmType::CONVEX_HULL:
        break;
    }

    bool hasFilters = parameters.hasFilters();
    const auto& filters = parameters.getFilters();

    if (hasFilters)
    {
        mResults.addBiomassMetaInfo("");
        mResults.addBiomassMetaInfo("Biomass Applied Filters:");

        for (auto* filter : filters)
        {
            mResults.addBiomassMetaInfo(filter->info());
        }
    }

    auto n = mPlotInfo->size();

    for (std::size_t i = 0; i < n; ++i)
    {
        update_progress(mID, static_cast<int>((100.0 * i) / n));

        auto plot = *((*mPlotInfo)[i]);

        if (hasFilters)
        {
            for (auto* filter : filters)
            {
                filter->apply(plot);
            }
        }

        if (plot.vegetationOnly())
        {
            plot::trim_below_in_place(plot, groundLevelBound_mm);
        }
        else if (plot.groundLevel_mm().has_value())
        {
            auto groundShift_mm = plot.groundLevel_mm().value();

            plot::trim_below_in_place(plot, groundShift_mm);

            plot::translate(plot, 0, 0, -groundShift_mm);
            plot.clearGroundLevel_mm();

            plot::trim_below_in_place(plot, groundLevelBound_mm);
        }
        else
        {
            plot::trim_below_in_place(plot, groundLevelBound_mm);
            plot::translate(plot, 0, 0, -groundLevelBound_mm);
        }

        double biomass = 0.0;

        switch (algorithm_type)
        {
        case eBiomassAlgorithmType::OCT_TREE:
            biomass = nPlotUtils::computeDigitalBiomass_oct_tree(plot, voxel_size_mm, min_bin_count);
            break;
        case eBiomassAlgorithmType::VOXEL_GRID:
            biomass = nPlotUtils::computeDigitalBiomass_voxel_grid(plot, voxel_size_mm, min_bin_count);
            break;
        case eBiomassAlgorithmType::CONVEX_HULL:
            biomass = nPlotUtils::computeDigitalBiomass_convex_hull(plot);
            break;
        }

        mResults.addPlotBiomass(plot.id(), mDayOfYear, biomass);
    }
}

void cFileProcessor::computePlotLAIs()
{
/*
    const auto& parameters = mConfigInfo.getLAI_Parameters();

    auto algorithm_type = parameters.getAlgorithmType();
    int groundLevelBound_mm = static_cast<int>(parameters.getGroundLevelBound_mm());
    double voxel_size_mm = parameters.getVoxelSize_mm();
    int min_bin_count = parameters.getMinBinCount();

    mResults.clearLAI_MetaInfo();
    switch (algorithm_type)
    {
    case eBiomassAlgorithmType::OCT_TREE:
        mResults.addBiomassMetaInfo("algorithm: voxelization (oct tree)");
        break;
    case eBiomassAlgorithmType::VOXEL_GRID:
        mResults.addBiomassMetaInfo("algorithm: voxelization (voxel grid)");
        break;
    case eBiomassAlgorithmType::CONVEX_HULL:
        mResults.addBiomassMetaInfo("algorithm: convex hull");
        break;
    default:
        mResults.addBiomassMetaInfo("algorithm: voxelization");
        break;
    }

    std::string info = "voxel size (mm): ";
    info += std::to_string(voxel_size_mm);
    mResults.addBiomassMetaInfo(info);

    switch (algorithm_type)
    {
    case eBiomassAlgorithmType::OCT_TREE:
        info = "min bin count: ";
        info += std::to_string(min_bin_count);
        mResults.addBiomassMetaInfo(info);
        break;
    case eBiomassAlgorithmType::VOXEL_GRID:
        if (min_bin_count > 0)
        {
            info = "min bin count: ";
            info += std::to_string(min_bin_count);
            mResults.addBiomassMetaInfo(info);
        }
        break;
    case eBiomassAlgorithmType::CONVEX_HULL:
        break;
    }

    bool hasFilters = parameters.hasFilters();
    const auto& filters = parameters.getFilters();

    if (hasFilters)
    {
        mResults.addBiomassMetaInfo("");
        mResults.addBiomassMetaInfo("Biomass Applied Filters:");

        for (auto* filter : filters)
        {
            mResults.addBiomassMetaInfo(filter->info());
        }
    }

    auto n = mPlotInfo->size();

    for (std::size_t i = 0; i < n; ++i)
    {
        update_progress(mID, static_cast<int>((100.0 * i) / n));

        auto plot = *((*mPlotInfo)[i]);

        if (hasFilters)
        {
            for (auto* filter : filters)
            {
                filter->apply(plot);
            }
        }

        if (plot.vegetationOnly())
        {
            plot::trim_below_in_place(plot, groundLevelBound_mm);
        }
        else if (plot.groundLevel_mm().has_value())
        {
            auto groundShift_mm = plot.groundLevel_mm().value();

            plot::trim_below_in_place(plot, groundShift_mm);

            plot::translate(plot, 0, 0, -groundShift_mm);
            plot.clearGroundLevel_mm();

            plot::trim_below_in_place(plot, groundLevelBound_mm);
        }
        else
        {
            plot::trim_below_in_place(plot, groundLevelBound_mm);
            plot::translate(plot, 0, 0, -groundLevelBound_mm);
        }

        double lai = 0.0;

        switch (algorithm_type)
        {
        case eBiomassAlgorithmType::OCT_TREE:
            biomass = nPlotUtils::computeDigitalBiomass_oct_tree(plot, voxel_size_mm, min_bin_count);
            break;
        case eBiomassAlgorithmType::VOXEL_GRID:
            biomass = nPlotUtils::computeDigitalBiomass_voxel_grid(plot, voxel_size_mm, min_bin_count);
            break;
        case eBiomassAlgorithmType::CONVEX_HULL:
            biomass = nPlotUtils::computeDigitalBiomass_convex_hull(plot);
            break;
        }

        mResults.addPlotLAI(plot.id(), mDayOfYear, biomass);
    }
*/
}


