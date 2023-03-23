//==============================================================================
// Copyright 2018-2020 Kitware, Inc., Kitware SAS
// Author: Guilbert Pierre (Kitware SAS)
//         Laurenson Nick (Kitware SAS)
//         Cadart Nicolas (Kitware SAS)
// Creation date: 2018-03-27
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//==============================================================================

#include "Utilities.h"
#include "SpinningSensorKeypointExtractor.h"

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include <pcl/common/common.h>

#include <random>

namespace LidarSlam
{

namespace
{
//-----------------------------------------------------------------------------
bool LineFitting::FitLineAndCheckConsistency(const SpinningSensorKeypointExtractor::PointCloud& cloud,
                                            const std::vector<int>& indices)
{
    // Check line width
    float lineLength = (cloud[indices.front()].getVector3fMap() - cloud[indices.back()].getVector3fMap()).norm();
    float widthTheshold = std::max(mMaxLineWidth, lineLength / mLengthWidthRatio);

    float maxDist = widthTheshold;
    Eigen::Vector3f bestDirection;

    mPosition = Eigen::Vector3f::Zero();
    for (int idx : indices)
        mPosition += cloud[idx].getVector3fMap();

    mPosition /= indices.size();

    // RANSAC
    for (int i = 0; i < int(indices.size()) - 1; ++i)
    {
        // Extract first point
        auto& point1 = cloud[indices[i]].getVector3fMap();
        for (int j = i+1; j < int(indices.size()); ++j)
        {
            // Extract second point
            auto& point2 = cloud[indices[j]].getVector3fMap();

            // Compute line formed by point1 and point2
            mDirection = (point1 - point2).normalized();

            // Reset score for new points pair
            float currentMaxDist = 0;
            // Compute score : maximum distance of one neighbor to the current line
            for (int idx : indices)
            {
                currentMaxDist = std::max(currentMaxDist, DistanceToPoint(cloud[idx].getVector3fMap()));

                // If the current point distance is too high,
                // the current line won't be selected anyway so we
                // can avoid computing next points' distances
                if (currentMaxDist > widthTheshold)
                    break;
            }

            // If the current line implies high error for one neighbor
            // the output line is considered as not trustworthy
            if (currentMaxDist > 2.0f * widthTheshold)
                return false;

            if (currentMaxDist <= maxDist)
            {
                bestDirection = mDirection;
                maxDist = currentMaxDist;
            }
        }
    }

    if (maxDist >= widthTheshold - 1e-10)
        return false;

    mDirection = bestDirection;

    return true;
}

    //-----------------------------------------------------------------------------
    inline float LineFitting::DistanceToPoint(Eigen::Vector3f const& point) const
    {
        return ((point - mPosition).cross(mDirection)).norm();
    }
} // end of anonymous namespace

//-----------------------------------------------------------------------------
void SpinningSensorKeypointExtractor::Enable(const std::vector<Keypoint>& kptTypes)
{
    for (auto& en : mEnabled)
        en.second = false;

    for (auto& k : kptTypes)
        mEnabled[k] = true;
}

//-----------------------------------------------------------------------------
SpinningSensorKeypointExtractor::PointCloud::Ptr SpinningSensorKeypointExtractor::GetKeypoints(Keypoint k)
{
    if (!mEnabled.count(k) || !mEnabled[k])
    {
        PRINT_ERROR("Unable to get keypoints of type " << KeypointTypeNames.at(k));
        return PointCloud::Ptr();
    }

    PointCloud::Ptr keypoints = mKeypoints.at(k).GetCloud(mMaxPoints);
    Utils::CopyPointCloudMetadata(*mpScan, *keypoints);
    return keypoints;
}


//-----------------------------------------------------------------------------
void SpinningSensorKeypointExtractor::ComputeKeyPoints(const PointCloud::Ptr& pc)
{
    mpScan = pc;

    // Split whole pointcloud into separate laser ring clouds
    ConvertAndSortScanLines();

    // Initialize the features vectors and keypoints
    PrepareDataForNextFrame();

    // Compute keypoints scores
    ComputeCurvature();

    // Labelize and extract keypoints
    // Warning : order matters
    if (mEnabled[Keypoint::BLOB])
        ComputeBlobs();

    if (mEnabled[Keypoint::PLANE])
        ComputePlanes();

    if (mEnabled[Keypoint::EDGE])
        ComputeEdges();

    if (mEnabled[Keypoint::INTENSITY_EDGE])
        ComputeIntensityEdges();
}

//-----------------------------------------------------------------------------
void SpinningSensorKeypointExtractor::ConvertAndSortScanLines()
{
    // Clear previous scan lines
    for (auto& scanLineCloud: mScanLines)
    {
        // Use clear() if pointcloud already exists to avoid re-allocating memory.
        // No worry as ScanLines is never shared with outer scope.
        if (scanLineCloud)
            scanLineCloud->clear();
        else
            scanLineCloud.reset(new PointCloud);
    }

    // Separate pointcloud into different scan lines
    for (const Point& point: *mpScan)
    {
        // Ensure that there are enough available scan lines
        while (point.laser_id >= mScanLines.size())
            mScanLines.emplace_back(new PointCloud);

        // Add the current point to its corresponding laser scan
        mScanLines[point.laser_id]->push_back(point);
    }

    // Save the number of lasers
    mNbLaserRings = mScanLines.size();

    // Estimate azimuthal resolution if not already done
    // or if the previous value found is not plausible
    // (because last scan was badly formed, e.g. lack of points)
    if ((mAzimuthalResolution_rad < 1e-6) || (M_PI/4.0 < mAzimuthalResolution_rad))
        EstimateAzimuthalResolution();
}

//-----------------------------------------------------------------------------
void SpinningSensorKeypointExtractor::PrepareDataForNextFrame()
{
    // Initialize the features vectors with the correct length
    mAngles.resize(mNbLaserRings);
    mDepthGap.resize(mNbLaserRings);
    mSpaceGap.resize(mNbLaserRings);
    mIntensityGap.resize(mNbLaserRings);
    mLabel.resize(mNbLaserRings);

    // Initialize the scan lines features vectors with the correct length
    #pragma omp parallel for num_threads(mNbThreads) schedule(guided)
    for (int scanLine = 0; scanLine < static_cast<int>(mNbLaserRings); ++scanLine)
    {
        size_t nbPoint = mScanLines[scanLine]->size();
        mLabel[scanLine].assign(nbPoint, KeypointFlags().reset());  // set all flags to 0
        mAngles[scanLine].assign(nbPoint, -1.);
        mDepthGap[scanLine].assign(nbPoint, -1.);
        mSpaceGap[scanLine].assign(nbPoint, -1.);
        mIntensityGap[scanLine].assign(nbPoint, -1.);
    }

    // Reset voxel grids
    LidarPoint minPt, maxPt;
    pcl::getMinMax3D(*mpScan, minPt, maxPt);

    for (auto k : KeypointTypes)
    {
        if (mKeypoints.count(k))
            mKeypoints[k].Clear();

        if (mEnabled[k])
            mKeypoints[k].Init(minPt.getVector3fMap(), maxPt.getVector3fMap(), mVoxelResolution_m, mpScan->size());
    }
}

//-----------------------------------------------------------------------------
void SpinningSensorKeypointExtractor::ComputeCurvature()
{
    // Compute useful const values to lighten the loop
    const float cosMinBeamSurfaceAngle = std::cos(Utils::Deg2Rad(mMinBeamSurfaceAngle_deg));
    const float cosMaxAzimuth = std::cos(1.5 * mAzimuthalResolution_rad);
    const float cosSpaceGapAngle = std::cos(mEdgeNbGapPoints * mAzimuthalResolution_rad);
    float minAzimuth_rad = Utils::Deg2Rad(mMinAzimuth_deg);
    float maxAzimuth_rad = Utils::Deg2Rad(mMaxAzimuth_deg);

    // Rescale angles in [0, 2pi]
    while (minAzimuth_rad < 0)
        minAzimuth_rad += 2 * M_PI;

    while (maxAzimuth_rad < 0)
        maxAzimuth_rad += 2 * M_PI;

    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(0.0, 1.0);

    // loop over scans lines
    #pragma omp parallel for num_threads(mNbThreads) schedule(guided)
    for (int scanLine = 0; scanLine < static_cast<int>(mNbLaserRings); ++scanLine)
    {
        // Useful shortcuts
        const PointCloud& scanLineCloud = *(mScanLines[scanLine]);
        const int Npts = scanLineCloud.size();

        // if the line is almost empty, skip it
        if (IsScanLineAlmostEmpty(Npts))
            continue;

        // Loop over points in the current scan line
        for (int index = 0; index < Npts; ++index)
        {
            // Random sampling to decrease keypoints extraction
            // computation time
            if (mInputSamplingRatio < 1.0f && dis(gen) > mInputSamplingRatio)
                continue;

            // Central point
            const Eigen::Vector3f& centralPoint = scanLineCloud[index].getVector3fMap();
            float centralDepth = centralPoint.norm();

            // Check distance to sensor
            if (centralDepth < mMinDistanceToSensor_m)
                continue;

            // Check azimuth angle
            if (std::abs(maxAzimuth_rad - minAzimuth_rad) < 2 * M_PI - 1e-6)
            {
                float cosAzimuth = centralPoint.x() / std::sqrt(std::pow(centralPoint.x(), 2) + std::pow(centralPoint.y(), 2));
                float azimuth = centralPoint.y() > 0? std::acos(cosAzimuth) : 2*M_PI - std::acos(cosAzimuth);
                if (minAzimuth_rad == maxAzimuth_rad)
                    continue;

                if (minAzimuth_rad < maxAzimuth_rad &&
                    (azimuth < minAzimuth_rad || azimuth > maxAzimuth_rad))
                    continue;

                if (minAzimuth_rad > maxAzimuth_rad &&
                    (azimuth < minAzimuth_rad && azimuth > maxAzimuth_rad))
                    continue;
            }

            // Fill left and right neighbors
            // Those points must be more numerous than MinNeighNb and occupy more space than MinNeighRadius
            std::vector<int> leftNeighbors;
            int idxNeigh = 1;
            float lineLength = 0.0f;

            while ((int(leftNeighbors.size()) < mMinNeighNb
                    || lineLength < mMinNeighRadius)
                    && int(leftNeighbors.size()) < Npts)
            {
                leftNeighbors.push_back((index - idxNeigh + Npts) % Npts);
                lineLength = (scanLineCloud[leftNeighbors.back()].getVector3fMap() - scanLineCloud[leftNeighbors.front()].getVector3fMap()).norm();
                ++idxNeigh;
            }

            std::vector<int> rightNeighbors;
            idxNeigh = 1;
            lineLength = 0.0f;
            while ((int(rightNeighbors.size()) < mMinNeighNb
                    || lineLength < mMinNeighRadius)
                && int(rightNeighbors.size()) < Npts)
            {
                rightNeighbors.push_back((index + idxNeigh) % Npts);
                lineLength = (scanLineCloud[rightNeighbors.back()].getVector3fMap() - scanLineCloud[rightNeighbors.front()].getVector3fMap()).norm();
                ++idxNeigh;
            }

            const auto& rightPt = scanLineCloud[rightNeighbors.front()].getVector3fMap();
            const auto& leftPt = scanLineCloud[leftNeighbors.front()].getVector3fMap();

            const float rightDepth = rightPt.norm();
            const float leftDepth = leftPt.norm();

            const float cosAngleRight = std::abs(rightPt.dot(centralPoint) / (rightDepth * centralDepth));
            const float cosAngleLeft = std::abs(leftPt.dot(centralPoint) / (leftDepth * centralDepth));

            const Eigen::Vector3f diffVecRight = rightPt - centralPoint;
            const Eigen::Vector3f diffVecLeft = leftPt - centralPoint;

            const float diffRightNorm = diffVecRight.norm();
            const float diffLeftNorm = diffVecLeft.norm();

            float cosBeamLineAngleLeft = std::abs(diffVecLeft.dot(centralPoint) / (diffLeftNorm * centralDepth) );
            float cosBeamLineAngleRight = std::abs(diffVecRight.dot(centralPoint) / (diffRightNorm * centralDepth));

            if (mEnabled[EDGE])
            {
                // Compute space gap

                // Init variables
                float distRight = -1.0f;
                float distLeft = -1.0f;

                // Compute space gap (if some neighbors were missed)
                if (cosBeamLineAngleRight < cosMinBeamSurfaceAngle && cosAngleRight < cosSpaceGapAngle)
                    distRight = diffRightNorm;

                if (cosBeamLineAngleLeft < cosMinBeamSurfaceAngle && cosAngleLeft < cosSpaceGapAngle)
                    distLeft = diffLeftNorm;

                mSpaceGap[scanLine][index] = std::max(distLeft, distRight);

                // Stop search for first and last points of the scan line
                // because the discontinuity may alter the other criteria detection
                if (index < int(leftNeighbors.size()) || index >= Npts - int(rightNeighbors.size()))
                    continue;

                // Compute depth gap

                // Reinit variables
                distRight = -1.0f;
                distLeft = -1.0f;

                if (cosAngleRight > cosMaxAzimuth)
                    distRight = centralPoint.dot(diffVecRight) / centralDepth;

                if (cosAngleLeft > cosMaxAzimuth)
                    distLeft = leftPt.dot(diffVecLeft) / leftDepth;

                // Check right points are consecutive + not on a bended wall
                // If the points lay on a bended wall, previous and next points should be in the same direction
                if (distRight > mEdgeDepthGapThreshold_m)
                {
                    auto nextdiffVecRight = (scanLineCloud[rightNeighbors[1]].getVector3fMap() - rightPt).normalized();
                    if ((nextdiffVecRight.dot(diffVecRight) / diffRightNorm) > cosMinBeamSurfaceAngle ||
                        (-diffVecLeft.dot(diffVecRight) / (diffRightNorm * diffLeftNorm)) > cosMinBeamSurfaceAngle)
                    {
                        distRight = -1.0f;
                    }
                }

                // Check left points are consecutive + not on a bended wall
                // If the points lay on a bended wall, previous and next points should be in the same direction
                if (distLeft > mEdgeDepthGapThreshold_m)
                {
                    auto prevdiffVecLeft = (scanLineCloud[leftNeighbors[1]].getVector3fMap() - leftPt).normalized();
                    if ((prevdiffVecLeft.dot(diffVecLeft) / diffLeftNorm > cosMinBeamSurfaceAngle) ||
                        (-diffVecRight.dot(diffVecLeft) / (diffRightNorm * diffLeftNorm)) > cosMinBeamSurfaceAngle)
                    {
                        distLeft = -1.0f;
                    }
                }

                mDepthGap[scanLine][index] = std::max(distLeft, distRight);
            }

            if (cosAngleRight < cosMaxAzimuth || cosAngleLeft < cosMaxAzimuth)
                continue;

            // Fit line on the left and right neighborhoods and
            // skip point if they are not usable
            LineFitting leftLine, rightLine;
            if (!leftLine.FitLineAndCheckConsistency(scanLineCloud, leftNeighbors) ||
                !rightLine.FitLineAndCheckConsistency(scanLineCloud, rightNeighbors))
                continue;

            cosBeamLineAngleLeft = std::abs(leftLine.mDirection.dot(centralPoint) / centralDepth);
            if (cosBeamLineAngleLeft > cosMinBeamSurfaceAngle)
                continue;

            cosBeamLineAngleRight = std::abs(rightLine.mDirection.dot(centralPoint) / centralDepth);
            if (cosBeamLineAngleRight > cosMinBeamSurfaceAngle)
                continue;

            if (mEnabled[INTENSITY_EDGE])
            {
                // Compute intensity gap
                if (std::abs(scanLineCloud[rightNeighbors.front()].intensity - scanLineCloud[leftNeighbors.front()].intensity)
                     > mEdgeIntensityGapThreshold)
                {
                    // Compute mean intensity on the left
                    float meanIntensityLeft = 0;
                    for (int indexLeft : leftNeighbors)
                        meanIntensityLeft += scanLineCloud[indexLeft].intensity;

                    meanIntensityLeft /= leftNeighbors.size();

                    // Compute mean intensity on the right
                    float meanIntensityRight = 0;
                    for (int indexRight : rightNeighbors)
                        meanIntensityRight += scanLineCloud[indexRight].intensity;

                    meanIntensityRight /= rightNeighbors.size();
                
                    mIntensityGap[scanLine][index] = std::abs(meanIntensityLeft - meanIntensityRight);

                    // Remove neighbor points to get the best intensity discontinuity locally
                    if (mIntensityGap[scanLine][index-1] < mIntensityGap[scanLine][index])
                        mIntensityGap[scanLine][index-1] = -1;
                    else
                        mIntensityGap[scanLine][index] = -1;
                }
            }

            // Check point is not too far from the fitted lines before computing an angle
            if ((leftLine.DistanceToPoint(centralPoint) > mMaxDistance_m)
                || (rightLine.DistanceToPoint(centralPoint) > mMaxDistance_m))
            {
                continue;
            }

            if (mEnabled[PLANE] || mEnabled[EDGE])
            {
                // Compute angles
                mAngles[scanLine][index] = (leftLine.mDirection.cross(rightLine.mDirection)).norm();

                // Remove previous point from angle inspection if the angle is not maximal locally
                if (mEnabled[EDGE] && mAngles[scanLine][index] > mEdgeSinAngleThreshold)
                {
                    // Check previously computed angle to keep only the maximum angle keypoint locally
                    for (int indexLeft : leftNeighbors)
                    {
                        if (mAngles[scanLine][indexLeft] <= mAngles[scanLine][index])
                            mAngles[scanLine][indexLeft] = -1;
                        else
                        {
                            mAngles[scanLine][index] = -1;
                            break;
                        }
                    }
                }
            }
        } // Loop on points
    } // Loop on scanlines
}

//-----------------------------------------------------------------------------
void SpinningSensorKeypointExtractor::AddKptsUsingCriterion (Keypoint k,
                                                                const std::vector<std::vector<float>>& values,
                                                                float threshold,
                                                                bool threshIsMax,
                                                                double weightBasis)
{
    // Loop over the scan lines
    for (int scanlineIdx = 0; scanlineIdx < static_cast<int>(mNbLaserRings); ++scanlineIdx)
    {
        const int Npts = mScanLines[scanlineIdx]->size();

        // If the line is almost empty, skip it
        if (IsScanLineAlmostEmpty(Npts))
            continue;

        // If threshIsMax : ascending order (lowest first)
        // If threshIsMin : descending order (greatest first)
        std::vector<size_t> sortedValuesIndices = Utils::SortIdx(values[scanlineIdx], threshIsMax);

        for (const auto& index: sortedValuesIndices)
        {
            // If the point was already picked, or is invalid, continue
            if (mLabel[scanlineIdx][index][k] || values[scanlineIdx][index] < 0)
                continue;

            // Check criterion threshold
            bool valueAboveThresh = values[scanlineIdx][index] > threshold;

            // If criterion is not respected, break loop as indices are sorted.
            if ((threshIsMax && valueAboveThresh) ||
                (!threshIsMax && !valueAboveThresh))
                break;

            // The points with the lowest weight have priority for extraction
            float weight = threshIsMax ?
                weightBasis + values[scanlineIdx][index] / threshold :
                weightBasis - values[scanlineIdx][index] / values[scanlineIdx][0];

            // Indicate the type of the keypoint to debug and to exclude double edges
            mLabel[scanlineIdx][index].set(k);

            // Add keypoint
            mKeypoints[k].AddPoint(mScanLines[scanlineIdx]->at(index), weight);
        }
    }
}

//-----------------------------------------------------------------------------
void SpinningSensorKeypointExtractor::ComputePlanes()
{
    AddKptsUsingCriterion(Keypoint::PLANE, mAngles, mPlaneSinAngleThreshold);
}

//-----------------------------------------------------------------------------
void SpinningSensorKeypointExtractor::ComputeEdges()
{
    AddKptsUsingCriterion(Keypoint::EDGE, mDepthGap, mEdgeDepthGapThreshold_m, false, 1);
    AddKptsUsingCriterion(Keypoint::EDGE, mAngles, mEdgeSinAngleThreshold, false, 2);
    AddKptsUsingCriterion(Keypoint::EDGE, mSpaceGap, mEdgeDepthGapThreshold_m, false, 3);
}

//-----------------------------------------------------------------------------
void SpinningSensorKeypointExtractor::ComputeIntensityEdges()
{
    AddKptsUsingCriterion(Keypoint::INTENSITY_EDGE, mIntensityGap, mEdgeIntensityGapThreshold, false);
}

//-----------------------------------------------------------------------------
void SpinningSensorKeypointExtractor::ComputeBlobs()
{
    for (unsigned int scanLine = 0; scanLine < mNbLaserRings; ++scanLine)
    {
        for (unsigned int index = 0; index < mScanLines[scanLine]->size(); ++index)
            mKeypoints[Keypoint::BLOB].AddPoint(mScanLines[scanLine]->at(index));
    }
}

//-----------------------------------------------------------------------------
void SpinningSensorKeypointExtractor::EstimateAzimuthalResolution()
{
    // Compute horizontal angle values between successive points
    std::vector<float> angles;
    angles.reserve(mpScan->size());

    for (const PointCloud::Ptr& scanLine : mScanLines)
    {
        for (unsigned int index = 1; index < scanLine->size(); ++index)
        {
            // Compute horizontal angle between two measurements
            // WARNING: to be correct, the points need to be in the LIDAR sensor
            // coordinates system, where the sensor is spinning around Z axis.
            Eigen::Map<const Eigen::Vector2f> p1(scanLine->at(index - 1).data);
            Eigen::Map<const Eigen::Vector2f> p2(scanLine->at(index).data);
            float angle = std::abs(std::acos(p1.dot(p2) / (p1.norm() * p2.norm())));

            // Keep only angles greater than 0 to avoid dual return issues
            if (angle > 1e-4)
            angles.push_back(angle);
        }
    }

    // A minimum number of angles is needed to get a trustable estimator
    if (angles.size() < 100)
    {
        PRINT_WARNING("Not enough points to estimate azimuthal resolution");
        return;
    }

    // Estimate azimuthal resolution from these angles
    std::sort(angles.begin(), angles.end());
    unsigned int maxInliersIdx = angles.size();
    float maxAngle = Utils::Deg2Rad(5.0);
    float medianAngle = 0.0f;

    // Iterate until only angles between direct LiDAR beam neighbors remain.
    // The max resolution angle is decreased at each iteration.
    while (maxAngle > 1.8 * medianAngle)
    {
        maxInliersIdx = std::upper_bound(angles.begin(), angles.begin() + maxInliersIdx, maxAngle) - angles.begin();
        medianAngle = angles[maxInliersIdx / 2];
        maxAngle = std::min(medianAngle * 2., maxAngle / 1.8);
    }

    mAzimuthalResolution_rad = medianAngle;
    std::cout << "LiDAR's azimuthal resolution estimated to " << Utils::Rad2Deg(mAzimuthalResolution_rad) << "°" << std::endl;
}

//-----------------------------------------------------------------------------
std::unordered_map<std::string, std::vector<float>> SpinningSensorKeypointExtractor::GetDebugArray() const
{
    auto get1DVector = [this](auto const& vector2d)
    {
        std::vector<float> v(mpScan->size());
        std::vector<int> indexByScanLine(mNbLaserRings, 0);
        for (unsigned int i = 0; i < mpScan->size(); i++)
        {
            const auto& laserId = mpScan->points[i].laser_id;
            v[i] = vector2d[laserId][indexByScanLine[laserId]];
            indexByScanLine[laserId]++;
        }
        return v;
    }; // end of lambda expression

    auto get1DVectorFromFlag = [this](auto const& vector2d, int flag)
    {
        std::vector<float> v(mpScan->size());
        std::vector<int> indexByScanLine(mNbLaserRings, 0);
        for (unsigned int i = 0; i < mpScan->size(); i++)
        {
            const auto& laserId = mpScan->points[i].laser_id;
            v[i] = vector2d[laserId][indexByScanLine[laserId]][flag];
            indexByScanLine[laserId]++;
        }
        return v;
    }; // end of lambda expression

    std::unordered_map<std::string, std::vector<float>> map;
    map["sin_angle"]      = get1DVector(mAngles);
    map["depth_gap"]      = get1DVector(mDepthGap);
    map["space_gap"]      = get1DVector(mSpaceGap);
    map["intensity_gap"]  = get1DVector(mIntensityGap);
    map["edge_keypoint"]  = get1DVectorFromFlag(mLabel, Keypoint::EDGE);
    map["intensity_edge_keypoint"]  = get1DVectorFromFlag(mLabel, Keypoint::INTENSITY_EDGE);
    map["plane_keypoint"] = get1DVectorFromFlag(mLabel, Keypoint::PLANE);
    map["blob_keypoint"]  = get1DVectorFromFlag(mLabel, Keypoint::BLOB);
    return map;
}

} // end of LidarSlam namespace