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

#pragma once

#include "LidarPoint.h"
#include "Enums.h"
#include "VoxelGrid.h"

#include <pcl/point_cloud.h>

#include <unordered_map>
#include <map>
#include <bitset>
#include <map>

#define SetMacro(name,type) void Set##name (type _arg) { name = _arg; }
#define GetMacro(name,type) type Get##name () const { return name; }

#define Setter(name,type) void Set##name (type _arg) { m##name = _arg; }
#define Getter(name,type) type Get##name () const { return m##name; }


namespace LidarSlam
{

namespace
{
    //-----------------------------------------------------------------------------
    struct LineFitting
    {
        using Point = LidarPoint;
        using PointCloud = pcl::PointCloud<Point>;

        //! Fitting using very local line and check if this local line is consistent
        //! in a more global neighborhood
        bool FitLineAndCheckConsistency(const PointCloud& cloud,
                                        const std::vector<int>& indices);

        //! Compute the squared distance of a point to the fitted line
        inline float DistanceToPoint(Eigen::Vector3f const& point) const;

        // Direction and position
        Eigen::Vector3f mDirection;
        Eigen::Vector3f mPosition;

        //! Max line width to be trustworthy for lines < 20cm
        float mMaxLineWidth = 0.02;  // [m]

        // Ratio between length and width to be trustworthy
        float mLengthWidthRatio = 10.0; // [.]
    };
} // End of anonymous namespace


class SpinningSensorKeypointExtractor
{
public:
    using Point = LidarPoint;
    using PointCloud = pcl::PointCloud<Point>;

    Getter(NbThreads, int)
    Setter(NbThreads, int)

    Getter(MaxPoints, int)
    Setter(MaxPoints, int)

    Getter(InputSamplingRatio, float)
    Setter(InputSamplingRatio, float)

    Getter(MinNeighNb, int)
    Setter(MinNeighNb, int)

    Getter(MinNeighRadius, float)
    Setter(MinNeighRadius, float)

    Getter(MinDistanceToSensor_m, float)
    Setter(MinDistanceToSensor_m, float)

    Getter(MinAzimuth_deg, float)
    Setter(MinAzimuth_deg, float)

    Getter(MaxAzimuth_deg, float)
    Setter(MaxAzimuth_deg, float)

    Getter(MinBeamSurfaceAngle_deg, float)
    Setter(MinBeamSurfaceAngle_deg, float)

    Getter(PlaneSinAngleThreshold, float)
    Setter(PlaneSinAngleThreshold, float)

    Getter(EdgeSinAngleThreshold, float)
    Setter(EdgeSinAngleThreshold, float)

    Getter(EdgeDepthGapThreshold_m, float)
    Setter(EdgeDepthGapThreshold_m, float)

    Getter(EdgeIntensityGapThreshold, float)
    Setter(EdgeIntensityGapThreshold, float)

    Getter(AzimuthalResolution_rad, float)
    Setter(AzimuthalResolution_rad, float)

    Getter(EdgeNbGapPoints, int)
    Setter(EdgeNbGapPoints, int)

    Getter(VoxelResolution_m, float)
    Setter(VoxelResolution_m, float)

    Getter(NbLaserRings, int)

    // Select the keypoint types to extract
    // This function resets the member map "Enabled"
    void Enable(const std::vector<Keypoint>& kptTypes);

    PointCloud::Ptr GetKeypoints(Keypoint k);

    // Extract keypoints from the pointcloud. The key points
    // will be separated in two classes : Edges keypoints which
    // correspond to area with high curvature scan lines and
    // planar keypoints which have small curvature.
    // NOTE: This expects that the lowest/bottom laser_id is 0, and is increasing upward.
    void ComputeKeyPoints(const PointCloud::Ptr& pc);

    // Function to enable to have some inside on why a given point was detected as a keypoint
    std::unordered_map<std::string, std::vector<float>> GetDebugArray() const;

private:
    // Split the whole pointcloud into separate laser ring clouds,
    // sorted by their vertical angles.
    // This expects that the lowest/bottom laser ring is 0, and is increasing upward.
    void ConvertAndSortScanLines();

    // Reset all the features vectors and keypoints clouds
    void PrepareDataForNextFrame();

    // Invalid the points with bad criteria from the list of possible future keypoints.
    // These points correspond to planar surfaces roughly parallel to laser beam
    // and points close to a gap created by occlusion.
    void InvalidateNotUsablePoints();

    // Compute the curvature and other features within each the scan line.
    // The curvature is not the one of the surface that intersects the lines but
    // the 1D curvature within each isolated scan line.
    void ComputeCurvature();

    // Labelize points (unvalid, edge, plane, blob)
    // and extract them in correspondant pointcloud
    void ComputePlanes();
    void ComputeEdges();
    void ComputeIntensityEdges();
    void ComputeBlobs();

    // Auto estimate azimuth angle resolution based on current ScanLines
    // WARNING: to be correct, the points need to be in the LIDAR sensor
    // coordinates system, where the sensor is spinning around Z axis.
    void EstimateAzimuthalResolution();

    // Check if scanLine is almost empty
    inline bool IsScanLineAlmostEmpty(int nScanLinePts) const { return nScanLinePts < 2 * mMinNeighNb + 1; }

    // Add all keypoints of the type k that comply with the threshold criteria for these values
    // The threshold can be a minimum or maximum value (threshIsMax)
    // The weight basis allow to weight the keypoint depending on its certainty
    void AddKptsUsingCriterion (Keypoint k,
                                const std::vector<std::vector<float>>& values,
                                float threshold,
                                bool threshIsMax = true,
                                double weightBasis = 1.);

    // ---------------------------------------------------------------------------
    //   Parameters
    // ---------------------------------------------------------------------------

    // Keypoints activated
    std::map<Keypoint, bool> mEnabled = {{EDGE, true}, {INTENSITY_EDGE, true}, {PLANE, true}, {BLOB, false}};

    // Max number of threads to use to process points in parallel
    int mNbThreads = 1;

    // Maximum number of keypoints to extract
    int mMaxPoints = INT_MAX;

    // Sampling ratio to perform for real time issues
    float mInputSamplingRatio = 1.0f;

    // Minimum number of points used on each side of the studied point to compute its curvature
    int mMinNeighNb = 4;

    // Minimum radius to define the neighborhood to compute curvature of a studied point
    int mMinNeighRadius = 0.05f;

    // Minimal point/sensor sensor to consider a point as valid
    float mMinDistanceToSensor_m = 1.5f;  // [m]

    // Minimum angle between laser beam and surface to consider a point as valid
    float mMinBeamSurfaceAngle_deg = 10; // [°]

    float mMinAzimuth_deg = 0.0f; // [°]
    float mMaxAzimuth_deg = 360.0f; // [°]

    // Sharpness threshold to select a planar keypoint
    float mPlaneSinAngleThreshold = 0.5;  // sin(30°) (selected if sin angle is less than threshold)

    // Sharpness threshold to select an edge keypoint
    float mEdgeSinAngleThreshold = 0.86;  // ~sin(60°) (selected, if sin angle is more than threshold)
    float mMaxDistance_m = 0.20;  // [m]

    // Threshold upon depth gap in neighborhood to select an edge keypoint
    float mEdgeDepthGapThreshold_m = 0.5;  // [m]

    // Threshold upon intensity gap to select an edge keypoint
    float mEdgeIntensityGapThreshold = 50.0f;

    // Nb of points missed to define a space gap
    int mEdgeNbGapPoints = 5; // [nb]

    // Size of a voxel used to downsample the keypoints
    // It corresponds approx to the mean distance between closest neighbors in the output keypoints cloud.
    float mVoxelResolution_m = 0.1; // [m]

    // ---------------------------------------------------------------------------
    //   Internal variables
    // ---------------------------------------------------------------------------

    // Azimuthal (= horizontal angle) resolution of the spinning lidar sensor
    // If it is less or equal to 0, it will be auto-estimated from next frame.
    // This angular resolution is used to compute an expected distance between two
    // consecutives firings.
    float mAzimuthalResolution_rad = 0.0f;  // [rad]

    // Number of lasers scan lines composing the pointcloud
    unsigned int mNbLaserRings = 0;

    //! Label of a point as a keypoint
    //! We use binary flags as each point can have different keypoint labels.
    using KeypointFlags = std::bitset<Keypoint::nKeypointTypes>;

    // Curvature and other differential operations (scan by scan, point by point)
    std::vector<std::vector<float>> mAngles;
    std::vector<std::vector<float>> mDepthGap;
    std::vector<std::vector<float>> mSpaceGap;
    std::vector<std::vector<float>> mIntensityGap;
    std::vector<std::vector<KeypointFlags>> mLabel;

    // Extracted keypoints of current frame
    std::map<Keypoint, VoxelGrid> mKeypoints;

    // Current point cloud stored in two differents formats
    PointCloud::Ptr mpScan;
    std::vector<PointCloud::Ptr> mScanLines;
};

} // end of LidarSlam namespace