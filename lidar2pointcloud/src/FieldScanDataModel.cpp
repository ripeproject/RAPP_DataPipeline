
#include "FieldScanDataModel.hpp"

#include "FieldScanLoader.hpp"
#include "PointCloudGenerator.hpp"


cFieldScanDataModel::cFieldScanDataModel()
{
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

const std::string& cFieldScanDataModel::getFileName()
{
    return mFilename;
}

void cFieldScanDataModel::clear()
{
    mScanTime_sec = 0.0;
    mFilename.clear();
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
std::shared_ptr<cExperimentInfo> cFieldScanDataModel::getExperimentInfo()
{
	return mExperimentInfo;
}

std::shared_ptr<cSpiderCamInfo> cFieldScanDataModel::getSpiderCamInfo()
{
    return mSpiderCamInfo;
}

std::shared_ptr<cSsnxInfo> cFieldScanDataModel::getSsnxInfo()
{
    return mSsnxInfo;
}

std::shared_ptr<cOusterInfo> cFieldScanDataModel::getOusterInfo()
{
    return mOusterInfo;
}

void cFieldScanDataModel::loadFieldScanData(const std::string& filename)
{
    if (!mFieldScanLoader)
    {
        mFieldScanLoader = std::make_unique<cFieldScanLoader>(*this);
    }

    if (!mFieldScanLoader->loadFile(filename))
        return;

    clear();

/*
    QString str = "Loading ";
    str += QString::fromStdString(filename);
    str += "...";

    QProgressDialog* progress = new QProgressDialog(str, "Abort", 0, 100, QApplication::activeModalWidget(), Qt::WindowStaysOnTopHint);
    progress->setWindowModality(Qt::WindowModal);
    progress->setMinimumDuration(0);

    connect(progress, &QProgressDialog::canceled, mFieldScanLoader.get(), &cFieldScanLoader::abort);
    connect(mFieldScanLoader.get(), &cFieldScanLoader::progressUpdated, progress, &QProgressDialog::setValue);
    connect(mFieldScanLoader.get(), &cFieldScanLoader::loadingComplete, this, &cRappDataModel::onFileLoadComplete);
    connect(this, &cRappDataModel::closeProgressDialog, progress, &QObject::deleteLater);
*/

    mFieldScanLoader->run();

    mFilename = filename;
}

/*
void cRappDataModel::onFileLoadComplete()
{
    QString title = QString::fromStdString(mExperimentInfo->title());
    emit onExperimentTitle(title);

    auto ouster = mOusterInfo->getPointCloudGenerator().lock();
    mScanTime_sec = ouster->getScanTime_sec();

    if (mScanTime_sec <= 0.0)
    {
        double updateRate_Hz = mOusterInfo->getUpdateRate_Hz();
        
        ouster->fixTimestampInfo(updateRate_Hz);

        mScanTime_sec = ouster->getScanTime_sec();
    }

    bool hasLidarData = ouster->hasData();
    if (hasLidarData)
    {
        emit onHasLidarData();
    }

    bool hasSpidercamInfo = mSpiderCamInfo->hasPositionData();
    if (hasSpidercamInfo)
    {
        emit onHasSpiderData();

        mSpiderCamInfo->findStartAndEndIndex();

        auto start = mSpiderCamInfo->startPosition();
        emit onStartPosition(start.X_mm, start.Y_mm, start.Z_mm, mSpiderCamInfo->getStartIndex());

        auto end = mSpiderCamInfo->endPosition();
        emit onEndPosition(end.X_mm, end.Y_mm, end.Z_mm, mSpiderCamInfo->getEndIndex());

        double time_sec = ((end.timestamp - start.timestamp) * 0.1) * nConstants::US_TO_SEC;
        emit onScanTimeUpdate(mScanTime_sec);
    }
    else
    {
        emit onScanTimeUpdate(mScanTime_sec);
    }

    double groundTrack_deg = mSsnxInfo->computeAvgGroundTrack_deg();
    bool hasGpsInfo = groundTrack_deg != cSsnxInfo::INVALID_GROUND_TRACK;
    if (hasGpsInfo)
    {
        emit onHasGpsData();
        emit onGroundTrackUpdate(groundTrack_deg);
    }

    emit closeProgressDialog();
}
*/

void cFieldScanDataModel::setStartIndex(std::size_t i)
{
    if (!mSpiderCamInfo->hasPositionData())
        return;

    mSpiderCamInfo->setStartIndex(i);

    auto start = mSpiderCamInfo->startPosition();
//    emit onStartPosition(start.X_mm, start.Y_mm, start.Z_mm, i);
}

void cFieldScanDataModel::setEndIndex(std::size_t i)
{
    if (!mSpiderCamInfo->hasPositionData())
        return;

    mSpiderCamInfo->setEndIndex(i);

    auto end = mSpiderCamInfo->endPosition();
//    emit onEndPosition(end.X_mm, end.Y_mm, end.Z_mm, i);
}
