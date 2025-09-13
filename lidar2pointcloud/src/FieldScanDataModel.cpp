
#include "FieldScanDataModel.hpp"

#include "FieldScanLoader.hpp"
#include "PointCloudGenerator.hpp"

#include "StringUtils.hpp"

extern void update_prefix_progress(const int id, std::string prefix, const int progress_pct);
extern void update_progress(const int id, const int progress_pct);


cFieldScanDataModel::cFieldScanDataModel(int id)
    : mID(id)
{
    mProcessingInfo = std::make_shared<cProcessingInfo>();
	mExperimentInfo = std::make_shared<cExperimentInfo>();
    mSpiderCamInfo = std::make_shared<cSpiderCamInfo>();
    mSsnxInfo = std::make_shared<cSsnxInfo>();
    mOusterInfo = std::make_shared<cOusterInfo>();
}

cFieldScanDataModel::~cFieldScanDataModel()
{
}

double cFieldScanDataModel::getScanTime_sec() const
{
    return mScanTime_sec;
}

double cFieldScanDataModel::getGroundTrack_deg() const
{
    return mGroundTrack_deg;
}

const std::string& cFieldScanDataModel::getFileName() const
{
    return mFilename;
}

const std::string& cFieldScanDataModel::getExperimentTitle() const
{
    return mExperimentTitle;
}

const std::string& cFieldScanDataModel::getMeasurementTitle() const
{
    return mMeasurementTitle;
}

void cFieldScanDataModel::clear()
{
    mScanTime_sec = 0.0;
    mGroundTrack_deg = cSsnxInfo::INVALID_GROUND_TRACK;
    mFilename.clear();
    mExperimentTitle.clear();
    mMeasurementTitle.clear();
    mProcessingInfo->clear();
	mExperimentInfo->clear();
    mSpiderCamInfo->clear();
    mSsnxInfo->clear();
    mOusterInfo->clear();
}

//-----------------------------------------------------------------------------
bool cFieldScanDataModel::hasLidarData() const
{
    return mOusterInfo->getImuData().size() > 0;
}

bool cFieldScanDataModel::hasSpiderData() const
{
    return mSpiderCamInfo->hasPositionData();
}

bool cFieldScanDataModel::hasGpsData() const
{
    return mSsnxInfo->getGeodeticPositions().size() > 0;
}

//-----------------------------------------------------------------------------
std::shared_ptr<cProcessingInfo> cFieldScanDataModel::getProcessingInfo()
{
    return mProcessingInfo;
}

std::shared_ptr<cProcessingInfo> cFieldScanDataModel::getProcessingInfo() const
{
    return mProcessingInfo;
}

std::shared_ptr<cExperimentInfo> cFieldScanDataModel::getExperimentInfo()
{
	return mExperimentInfo;
}

std::shared_ptr<cExperimentInfo> cFieldScanDataModel::getExperimentInfo() const
{
    return mExperimentInfo;
}

std::shared_ptr<cSpiderCamInfo> cFieldScanDataModel::getSpiderCamInfo()
{
    return mSpiderCamInfo;
}

std::shared_ptr<cSpiderCamInfo> cFieldScanDataModel::getSpiderCamInfo() const
{
    return mSpiderCamInfo;
}

std::shared_ptr<cSsnxInfo> cFieldScanDataModel::getSsnxInfo()
{
    return mSsnxInfo;
}

std::shared_ptr<cSsnxInfo> cFieldScanDataModel::getSsnxInfo() const
{
    return mSsnxInfo;
}

std::shared_ptr<cOusterInfo> cFieldScanDataModel::getOusterInfo()
{
    return mOusterInfo;
}

std::shared_ptr<cOusterInfo> cFieldScanDataModel::getOusterInfo() const
{
    return mOusterInfo;
}

void cFieldScanDataModel::loadFieldScanData(const std::string& filename)
{
    std::unique_ptr<cFieldScanLoader> fieldScanLoader = std::make_unique<cFieldScanLoader>(mID, *this);

    if (!fieldScanLoader->loadFile(filename))
        return;

    clear();

    update_prefix_progress(mID, "Loading data...", 0);

    if (!fieldScanLoader->run())
        return;

    mFilename = filename;

    mExperimentTitle = mExperimentInfo->experimentTitle();
    mMeasurementTitle = mExperimentInfo->measurementTitle();

    if (mExperimentTitle.empty())
    {
        nStringUtils::sPathAndFilename pf = nStringUtils::splitPathname(mFilename);
        auto fe = nStringUtils::removeMeasurementTimestamp(pf.filename);
        mExperimentInfo->setExperimentTitle(fe.filename);

        mExperimentTitle = fe.filename;
    }

    auto ouster = mOusterInfo->getPointCloudGenerator().lock();
    mScanTime_sec = ouster->getScanTime_sec();

    if (mScanTime_sec <= 0.0)
    {
        double updateRate_Hz = mOusterInfo->getUpdateRate_Hz();

        ouster->fixTimestampInfo(updateRate_Hz);

        mScanTime_sec = ouster->getScanTime_sec();
    }

    bool hasSpidercamInfo = mSpiderCamInfo->hasPositionData();
    if (hasSpidercamInfo)
    {
        mSpiderCamInfo->findStartAndEndIndex(2.5);
    }

    mGroundTrack_deg = mSsnxInfo->computeAvgGroundTrack_deg();
}

void cFieldScanDataModel::setScanTime_sec(double time_sec)
{
    mScanTime_sec = time_sec;
}

void cFieldScanDataModel::setGroundTrack_deg(double track_deg)
{
    mGroundTrack_deg = track_deg;
}

void cFieldScanDataModel::validateStartPosition(int32_t x_mm, int32_t y_mm, int32_t z_mm, uint32_t tolerence_mm)
{
    // All valid SpiderCam positions are positive, except the z-axis
    if ((x_mm < 0) || (y_mm < 0) || (z_mm < -1000))
        return;

    // Not a valid position
    if ((x_mm == 0) && (y_mm == 0) && (z_mm == 0))
        return;

    if (!mSpiderCamInfo->hasPositionData())
        return;

    auto start = mSpiderCamInfo->startPosition();

    double dx = start.X_mm - x_mm;
    double dy = start.Y_mm - y_mm;
    double dz = start.Z_mm - z_mm;

    double d = sqrt(dx * dx + dy * dy + dz * dz);

    if (d < tolerence_mm) return;

    auto positions = mSpiderCamInfo->getPositionData();

    for (int i = 0; i < positions.size(); ++i)
    {
        auto pos = positions[i];
        double dx = pos.X_mm - x_mm;
        double dy = pos.Y_mm - y_mm;
        double dz = pos.Z_mm - z_mm;

        double d = sqrt(dx * dx + dy * dy + dz * dz);

        if (d < tolerence_mm)
        {
            mSpiderCamInfo->setStartIndex(i);
        }
    }
}

void cFieldScanDataModel::validateEndPosition(int32_t x_mm, int32_t y_mm, int32_t z_mm, uint32_t tolerence_mm)
{
    // All valid SpiderCam positions are positive, except the z-axis
    if ((x_mm < 0) || (y_mm < 0) || (z_mm < -1000))
        return;

    // Not a valid position
    if ((x_mm == 0) && (y_mm == 0) && (z_mm == 0))
        return;

    if (!mSpiderCamInfo->hasPositionData())
        return;

    auto end = mSpiderCamInfo->endPosition();

    double dx = end.X_mm - x_mm;
    double dy = end.Y_mm - y_mm;
    double dz = end.Z_mm - z_mm;

    double d = sqrt(dx * dx + dy * dy + dz * dz);

    if (d < tolerence_mm) return;

    auto positions = mSpiderCamInfo->getPositionData();

    for (int i = positions.size() - 1; i > 0; --i)
    {
        auto pos = positions[i];
        double dx = pos.X_mm - x_mm;
        double dy = pos.Y_mm - y_mm;
        double dz = pos.Z_mm - z_mm;

        double d = sqrt(dx * dx + dy * dy + dz * dz);

        if (d < tolerence_mm)
        {
            mSpiderCamInfo->setEndIndex(i);
        }
    }
}

