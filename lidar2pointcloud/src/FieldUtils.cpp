
#include "FieldUtils.hpp"

#include <dlib/optimization.h>

#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/search/kdtree.h> // for KdTree
#include <pcl/features/normal_3d.h>
#include <pcl/surface/gp3.h>

#include <pcl/io/vtk_io.h>

#include <vtkDelaunay2D.h>
#include <vtkDoubleArray.h>
#include <vtkTable.h>
#include <vtkTableToPolyData.h>

#include <array>

#include "Constants.hpp"

namespace
{
    constexpr std::size_t MIN_SET_OF_POINTS = 10;

    template<typename T>
    T threshold(T min, T max, double level_pct)
    {
        if (level_pct > 100.0) return max;
        if (level_pct < 0.0) return min;

        return static_cast<T>((max - min) * (level_pct / 100.0)) + min;
    }
}


void test(const std::vector<rfm::rappPoint_t>& ground_points)
{
	// Load input file into a PointCloud<T> with an appropriate type
	auto point_cloud = new pcl::PointCloud<pcl::PointXYZ>;

	for( const auto& ground_point : ground_points)
	{
		pcl::PointXYZ point;
		point.x = ground_point.x_mm;
		point.y = ground_point.y_mm;
		point.z = ground_point.z_mm;

		point_cloud->push_back(point);
	}

	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(point_cloud);
	//* the data should be available in cloud

	// Normal estimation*
	pcl::NormalEstimation<pcl::PointXYZ, pcl::Normal> n;
	pcl::PointCloud<pcl::Normal>::Ptr normals(new pcl::PointCloud<pcl::Normal>);
	pcl::search::KdTree<pcl::PointXYZ>::Ptr tree(new pcl::search::KdTree<pcl::PointXYZ>);
	tree->setInputCloud(cloud);
	n.setInputCloud(cloud);
	n.setSearchMethod(tree);
	n.setKSearch(20);
	n.compute(*normals);
	//* normals should not contain the point normals + surface curvatures

	// Concatenate the XYZ and normal fields*
	pcl::PointCloud<pcl::PointNormal>::Ptr cloud_with_normals(new pcl::PointCloud<pcl::PointNormal>);
	pcl::concatenateFields(*cloud, *normals, *cloud_with_normals);
	//* cloud_with_normals = cloud + normals

	// Create search tree*
	pcl::search::KdTree<pcl::PointNormal>::Ptr tree2(new pcl::search::KdTree<pcl::PointNormal>);
	tree2->setInputCloud(cloud_with_normals);

	// Initialize objects
	pcl::GreedyProjectionTriangulation<pcl::PointNormal> gp3;
	pcl::PolygonMesh triangles;

	// Set the maximum distance between connected points (maximum edge length)
	gp3.setSearchRadius(3000);

	// Set typical values for the parameters
	gp3.setMu(2.5);
	gp3.setMaximumNearestNeighbors(100);
	gp3.setMaximumSurfaceAngle(M_PI / 4); // 45 degrees
	gp3.setMinimumAngle(M_PI / 18); // 10 degrees
	gp3.setMaximumAngle(2 * M_PI / 3); // 120 degrees
	gp3.setNormalConsistency(false);

	// Get result
	gp3.setInputCloud(cloud_with_normals);
	gp3.setSearchMethod(tree2);
	gp3.reconstruct(triangles);

	// Additional vertex information
	std::vector<int> parts = gp3.getPartIDs();
	std::vector<int> states = gp3.getPointStates();

	pcl::io::saveVTKFile("mesh.vtk", triangles);
}


//-----------------------------------------------------------------------------
std::vector<cRappTriangle> computeGroundMesh(const std::vector<rfm::rappPoint_t>& ground_points)
{
    vtkDoubleArray* x = vtkDoubleArray::New();
    vtkDoubleArray* y = vtkDoubleArray::New();
    vtkDoubleArray* z = vtkDoubleArray::New();

    for (const auto& ground_point : ground_points)
    {
        x->InsertNextTuple1(ground_point.x_mm);
        y->InsertNextTuple1(ground_point.y_mm);
        z->InsertNextTuple1(ground_point.z_mm);
    }

    vtkNew<vtkTable> table;
    table->AddColumn(x);
    table->AddColumn(y);
    table->AddColumn(z);

    // Convert to a table view of the data
    vtkNew<vtkTableToPolyData> tablePoints;
    tablePoints->SetInputData(table);
    tablePoints->SetXColumnIndex(0);
    tablePoints->SetYColumnIndex(1);
    tablePoints->SetZColumnIndex(2);
    tablePoints->Update();

    // Triangulate the grid points.
    vtkNew<vtkDelaunay2D> delaunay;
    delaunay->SetInputData(tablePoints->GetOutput());
    delaunay->Update();
    vtkSmartPointer<vtkPolyData> mesh = delaunay->GetOutput();

    std::vector<cRappTriangle> data;

    auto m = mesh->GetNumberOfCells();
    vtkIdList* pts = vtkIdList::New();
    for (vtkIdType i = 0; i < m; ++i)
    {
        mesh->GetCellPoints(i, pts);
        auto p1_id = pts->GetId(0);
        auto p2_id = pts->GetId(1);
        auto p3_id = pts->GetId(2);

        double d_mm = (ground_points[p1_id].x_mm - ground_points[p2_id].x_mm) * (ground_points[p1_id].x_mm - ground_points[p2_id].x_mm);
        d_mm += (ground_points[p1_id].y_mm - ground_points[p2_id].y_mm) * (ground_points[p1_id].y_mm - ground_points[p2_id].y_mm);
        d_mm = sqrt(d_mm);

        if (d_mm > 10000)
        {
            continue;
        }

        d_mm = (ground_points[p1_id].x_mm - ground_points[p3_id].x_mm) * (ground_points[p1_id].x_mm - ground_points[p3_id].x_mm);
        d_mm += (ground_points[p1_id].y_mm - ground_points[p3_id].y_mm) * (ground_points[p1_id].y_mm - ground_points[p3_id].y_mm);
        d_mm = sqrt(d_mm);

        if (d_mm > 10000)
        {
            continue;
        }

        d_mm = (ground_points[p3_id].x_mm - ground_points[p2_id].x_mm) * (ground_points[p3_id].x_mm - ground_points[p2_id].x_mm);
        d_mm += (ground_points[p3_id].y_mm - ground_points[p2_id].y_mm) * (ground_points[p3_id].y_mm - ground_points[p2_id].y_mm);
        d_mm = sqrt(d_mm);

        if (d_mm > 10000)
        {
            continue;
        }

        data.emplace_back(ground_points[p1_id], ground_points[p2_id], ground_points[p3_id]);
    }

    pts->Delete();

    return std::move(data);
}

//-----------------------------------------------------------------------------
typedef dlib::matrix<double, 4, 1> input_vector;
typedef dlib::matrix<double, 1, 1> input_one_variable;
typedef dlib::matrix<double, 3, 1> input_three_variables;
typedef dlib::matrix<double, 2, 1> parameter_vector;
typedef dlib::matrix<double, 1, 1> pitch_vector;
typedef dlib::matrix<double, 1, 1> roll_vector;


double modelLine(const input_one_variable& input, const parameter_vector& params)
{
    const double m = params(0);
    const double b = params(1);

    const double x = input(0);

    double c = m * x + b;

    return c;
}

double residualLine(const std::pair<input_one_variable, double>& data, const parameter_vector& params)
{
    return modelLine(data.first, params) - data.second;
}

sLineParameters_t fitHorizontalLine(const std::vector<rfm::rappPoint_t>& points)
{
    using namespace dlib;

    if (points.empty())
    {
        return sLineParameters_t();
    }

    parameter_vector line;
    line(0) = 0.0;  // Horizontal line has zero slope

    // The intercept will be the average y value
    double sum = 0;
    for (auto point : points)
    {
        sum += point.y_mm;
    }
    line(1) = sum / points.size();

    std::vector<std::pair<input_one_variable, double>> samples;

    input_one_variable input;

    for (auto& point : points)
    {
        input(0) = point.x_mm;

        const double output = point.y_mm;

        // save the pair
        samples.push_back(std::make_pair(input, output));
    }

    auto R2 = solve_least_squares_lm(objective_delta_stop_strategy(0.001, 50)/*.be_verbose()*/,
        residualLine,
        derivative(residualLine),
        samples,
        line);

    sLineParameters_t result;

    result.m = line(0);
    result.b = line(1);

    return result;
}

sLineParameters_t fitVerticalLine(const std::vector<rfm::rappPoint_t>& points)
{
    using namespace dlib;

    parameter_vector line;
    line = 1;


    std::vector<std::pair<input_one_variable, double>> samples;

    input_one_variable input;

    for (auto& point : points)
    {
        input(0) = point.y_mm;

        const double output = point.x_mm;

        // save the pair
        samples.push_back(std::make_pair(input, output));
    }

    auto R2 = solve_least_squares_lm(objective_delta_stop_strategy(0.001, 50)/*.be_verbose()*/,
        residualLine,
        derivative(residualLine),
        samples,
        line);

    sLineParameters_t result;

    result.m = line(0);
    result.b = line(1);

    return result;
}

sLineParameters_t fitLine(const std::vector<rfm::rappPoint_t>& points, const std::vector<rfm::rappPoint_t>& line)
{
    sLineParameters_t result;

    return result;
}

double groundheight_pitch_roll(const input_three_variables& input, const parameter_vector& params)
{
    const double pitch_rad = params(0) * nConstants::DEG_TO_RAD;
    const double roll_rad = params(1) * nConstants::DEG_TO_RAD;

    const double x = input(0);
    const double y = input(1);
    const double z = input(2);

    Eigen::AngleAxisd rollAngle(roll_rad, Eigen::Vector3d::UnitX());
    Eigen::AngleAxisd yawAngle(0, Eigen::Vector3d::UnitZ());
    Eigen::AngleAxisd pitchAngle(pitch_rad, Eigen::Vector3d::UnitY());

    Eigen::Quaternion<double> q = pitchAngle * rollAngle * yawAngle;
    Eigen::Matrix3d rotationMatrix = q.matrix();

    const auto& rZ = rotationMatrix.col(2);

    double h = x * rZ[0] + y * rZ[1] + z * rZ[2];

    return h;
}


//-----------------------------------------------------------------------------
double groundheight_pitch(const input_three_variables& input, const pitch_vector& params)
{
    const double pitch_rad = params(0) * nConstants::DEG_TO_RAD;

    const double x = input(0);
    const double y = input(1);
    const double z = input(2);

    Eigen::AngleAxisd rollAngle(0, Eigen::Vector3d::UnitX());
    Eigen::AngleAxisd yawAngle(0, Eigen::Vector3d::UnitZ());
    Eigen::AngleAxisd pitchAngle(pitch_rad, Eigen::Vector3d::UnitY());

    Eigen::Quaternion<double> q = pitchAngle * rollAngle * yawAngle;
    Eigen::Matrix3d rotationMatrix = q.matrix();

    const auto& rZ = rotationMatrix.col(2);

    double h = x * rZ[0] + y * rZ[1] + z * rZ[2];

    return h;
}

//-----------------------------------------------------------------------------
double groundheight_roll(const input_three_variables& input, const roll_vector& params)
{
    const double roll_rad = params(0) * nConstants::DEG_TO_RAD;

    const double x = input(0);
    const double y = input(1);
    const double z = input(2);

    Eigen::AngleAxisd rollAngle(roll_rad, Eigen::Vector3d::UnitX());
    Eigen::AngleAxisd yawAngle(0, Eigen::Vector3d::UnitZ());
    Eigen::AngleAxisd pitchAngle(0, Eigen::Vector3d::UnitY());

    Eigen::Quaternion<double> q = pitchAngle * rollAngle * yawAngle;
    Eigen::Matrix3d rotationMatrix = q.matrix();

    const auto& rZ = rotationMatrix.col(2);

    double h = x * rZ[0] + y * rZ[1] + z * rZ[2];

    return h;
}

// This function is the "residual" for a least squares problem.   It takes an input/output
// pair and compares it to the output of our model and returns the amount of error.  The idea
// is to find the set of parameters which makes the residual small on all the data pairs.

double residual_pitch_roll(
    const std::pair<input_three_variables, double>& data,
    const parameter_vector& params
)
{
    return groundheight_pitch_roll(data.first, params) - data.second;
}


//-----------------------------------------------------------------------------
double residual_pitch(
    const std::pair<input_three_variables, double>& data,
    const pitch_vector& params
)
{
    return groundheight_pitch(data.first, params) - data.second;
}

//-----------------------------------------------------------------------------
double residual_roll(
    const std::pair<input_three_variables, double>& data,
    const roll_vector& params
)
{
    return groundheight_roll(data.first, params) - data.second;
}


//-----------------------------------------------------------------------------
sPitchAndRoll_t fitPointCloudToGroundMesh(const cRappPointCloud& pc)
{
    using namespace dlib;

    parameter_vector angles;
    angles = 1;

    std::vector<std::pair<input_three_variables, double>> samples;

    input_three_variables input;

    auto centroid = pc.centroid();

    for (auto& point : pc)
    {
        input(0) = point.x_mm - centroid.x_mm;
        input(1) = point.y_mm - centroid.y_mm;
        input(2) = point.z_mm - centroid.z_mm;

        // Output is the target value we are fitting for
        const double output = point.h_mm - centroid.z_mm;

        // save the pair
        samples.push_back(std::make_pair(input, output));
    }

    auto R2 = solve_least_squares_lm(objective_delta_stop_strategy(0.001, 50)/*.be_verbose() */,
        residual_pitch_roll,
        derivative(residual_pitch_roll),
        samples,
        angles);

    sPitchAndRoll_t result;

    result.pitch_deg = angles(0);
    result.roll_deg = angles(1);
    result.R = sqrt(R2);
    result.valid = true;

    return result;
}


//-----------------------------------------------------------------------------
sPitch_t fitPointCloudPitchToGroundMesh_deg(const cRappPointCloud& pc)
{
    using namespace dlib;

    pitch_vector pitch;
    pitch = 1;

    std::vector<std::pair<input_three_variables, double>> samples;

    input_three_variables input;

    auto centroid = pc.centroid();

    for (auto& point : pc)
    {
        input(0) = point.x_mm - centroid.x_mm;
        input(1) = point.y_mm - centroid.y_mm;
        input(2) = point.z_mm - centroid.z_mm;

        // Output is the target value we are fitting for
        const double output = point.h_mm - centroid.z_mm;

        // save the pair
        samples.push_back(std::make_pair(input, output));
    }

    auto R2 = solve_least_squares_lm(objective_delta_stop_strategy(0.001, 50)/*.be_verbose()*/,
        residual_pitch,
        derivative(residual_pitch),
        samples,
        pitch);

    sPitch_t result;

    result.pitch_deg = pitch(0);
    result.R = sqrt(R2);
    result.valid = true;

    return result;
}


//-----------------------------------------------------------------------------
sRoll_t fitPointCloudRollToGroundMesh_deg(const cRappPointCloud& pc)
{
    using namespace dlib;

    roll_vector roll;
    roll = 1;

    std::vector<std::pair<input_three_variables, double>> samples;

    input_three_variables input;

    auto centroid = pc.centroid();

    for (auto& point : pc)
    {
        input(0) = point.x_mm - centroid.x_mm;
        input(1) = point.y_mm - centroid.y_mm;
        input(2) = point.z_mm - centroid.z_mm;

        // Output is the target value we are fitting for
        const double output = point.h_mm - centroid.z_mm;

        // save the pair
        samples.push_back(std::make_pair(input, output));
    }

    auto R2 = solve_least_squares_lm(objective_delta_stop_strategy(0.001, 50)/*.be_verbose()*/,
        residual_roll,
        derivative(residual_roll),
        samples,
        roll);

    sRoll_t result;

    result.roll_deg = roll(0);
    result.R = sqrt(R2);
    result.valid = true;

    return result;
}

//-----------------------------------------------------------------------------
sOffset_t computePcToGroundMeshDistance_mm(const cRappPointCloud& pc)
{
    double offset = 0;
    unsigned long long num = 0;

    for (auto& point : pc)
    {
        if (point.h_mm == rfm::INVALID_HEIGHT)
            continue;

        offset += (point.h_mm - point.z_mm);
        ++num;
    }

    if (num < MIN_SET_OF_POINTS)
        return sOffset_t();

    offset /= num;

    return { offset, true };
}

//-----------------------------------------------------------------------------
sOffset_t computePcToGroundMeshDistance_mm(const cRappPointCloud& pc, double threshold_pct)
{
    auto data = pc.data();

    int32_t z_threshold = threshold(pc.minZ_mm(), pc.maxZ_mm(), threshold_pct);

    std::sort(data.begin(), data.end(), [](rfm::sPoint3D_t a, rfm::sPoint3D_t b)
        {
            if (a.z_mm < b.z_mm) return true;

            return false;
        });

    double offset = 0;
    unsigned long long num = 0;

    for (auto& point : data)
    {
        if (point.h_mm == rfm::INVALID_HEIGHT)
            continue;

        if (point.z_mm > z_threshold)
            break;

        offset += (point.h_mm - point.z_mm);
        ++num;
    }

    if (num < MIN_SET_OF_POINTS)
        return sOffset_t();

    offset /= num;

    return { offset, true };
}

//-----------------------------------------------------------------------------
rfm::sCentroid_t computeCentroid(const cRappPointCloud::vCloud_t& data)
{
    auto n = data.size();

    double sum_x = 0.0;
    double sum_y = 0.0;
    double sum_z = 0.0;

    for (std::size_t i = 0; i < n; ++i)
    {
        auto point = data[i];

        auto x = point.x_mm;
        auto y = point.y_mm;
        auto z = point.z_mm;

        sum_x += x;
        sum_y += y;
        sum_z += z;
    }

    double x_mm = sum_x / n;
    double y_mm = sum_y / n;
    double z_mm = sum_z / n;

    return { x_mm , y_mm, z_mm };
}

//-----------------------------------------------------------------------------
sPitchAndRoll_t computePcToGroundMeshRotation_deg(const cRappPointCloud& pc)
{
    cRappPointCloud::vCloud_t data;

    auto dx = (pc.maxX_mm() - pc.minX_mm()) / 100;
    auto dy = (pc.maxY_mm() - pc.minY_mm()) / 100;

    auto x_start = pc.minX_mm() + dx / 2;
    auto y_start = pc.minY_mm() + dy / 2;

    auto x_end = pc.maxX_mm();
    auto y_end = pc.maxY_mm();

    for (auto y = y_start; y < y_end; y += dy)
    {
        for (auto x = x_start; x < x_end; x += dx)
        {
            auto p = pc.getPoint(x, y, 100);

            if ((p.z_mm == rfm::INVALID_HEIGHT) || (p.h_mm == rfm::INVALID_HEIGHT))
            {
                continue;
            }

            data.push_back(p);
        }
    }

    if (data.size() < MIN_SET_OF_POINTS)
        return sPitchAndRoll_t();

    // Compute the centroid of all of the points...
    rfm::sCentroid_t centroid = computeCentroid(data);

    cRappPointCloud temp(centroid, data);

    return fitPointCloudToGroundMesh(temp);
}

//-----------------------------------------------------------------------------
sPitchAndRoll_t computePcToGroundMeshRotation_deg(const cRappPointCloud& pc, double threshold_pct)
{
    auto data = pc.data();

    int32_t z_threshold = threshold(pc.minZ_mm(), pc.maxZ_mm(), threshold_pct);

    auto end_it = std::remove_if(data.begin(), data.end(), [z_threshold](rfm::sPoint3D_t a)
        {
            return (a.z_mm > z_threshold) || (a.h_mm == rfm::INVALID_HEIGHT);
        });

    data.erase(end_it, data.end());

    if (data.size() < MIN_SET_OF_POINTS)
        return sPitchAndRoll_t();

    // Compute the centroid of all of the points...
    rfm::sCentroid_t centroid = computeCentroid(data);

    cRappPointCloud temp(centroid, data);

    return fitPointCloudToGroundMesh(temp);
}

//-----------------------------------------------------------------------------
sOffset_t computePcToGroundMeshDistanceUsingGrid_mm(const cRappPointCloud& pc, double threshold_pct)
{
    const int NUM_X_GRIDS = 10;
    const int NUM_Y_GRIDS = 10;

    std::array<cRappPointCloud::vCloud_t, NUM_X_GRIDS* NUM_Y_GRIDS> grid_data;

    std::array<int, NUM_X_GRIDS> x_bounds;
    std::array<int, NUM_Y_GRIDS> y_bounds;

    auto dx = (pc.maxX_mm() - pc.minX_mm()) / NUM_X_GRIDS;
    auto dy = (pc.maxY_mm() - pc.minY_mm()) / NUM_Y_GRIDS;

    for (int i = 0; i < NUM_X_GRIDS; ++i)
    {
        x_bounds[i] = pc.minX_mm() + i * dx;
    }

    for (int i = 0; i < NUM_Y_GRIDS; ++i)
    {
        y_bounds[i] = pc.minY_mm() + i * dy;
    }

    // Split the point cloud into grid data
    for (auto point : pc)
    {
        int i = 0;
        for (; i < NUM_X_GRIDS; ++i)
        {
            if (point.x_mm < x_bounds[i])
            {
                break;
            }
        }
        --i;

        int j = 0;
        for (; j < NUM_Y_GRIDS; ++j)
        {
            if (point.y_mm < y_bounds[j])
            {
                break;
            }
        }
        --j;

        int n = i * NUM_Y_GRIDS + j;

        grid_data[n].push_back(point);
    }


    double offset = 0;
    unsigned long long num = 0;

    // Use the bottom most points ( < threashold_pct) in each grid block to compute offset
    for (auto block : grid_data)
    {
        if (block.empty())
            continue;

        int minZ_mm = std::numeric_limits<int>::max();
        int maxZ_mm = std::numeric_limits<int>::min();

        for (auto& point : block)
        {
            minZ_mm = std::min(minZ_mm, point.z_mm);
            maxZ_mm = std::max(maxZ_mm, point.z_mm);
        }

        int32_t z_threshold = threshold(minZ_mm, maxZ_mm, threshold_pct);

        std::sort(block.begin(), block.end(), [](rfm::sPoint3D_t a, rfm::sPoint3D_t b)
            {
                if (a.z_mm < b.z_mm) return true;

                return false;
            });

        for (auto& point : block)
        {
            if (point.h_mm == rfm::INVALID_HEIGHT)
                continue;

            if (point.z_mm > z_threshold)
                break;

            offset += (point.h_mm - point.z_mm);
            ++num;
        }
    }

    if (num < MIN_SET_OF_POINTS)
        return sOffset_t();

    offset /= num;

    return { offset,true };
}

//-----------------------------------------------------------------------------
sPitchAndRoll_t computePcToGroundMeshRotationUsingGrid_deg(const cRappPointCloud& pc, double threshold_pct)
{
    const int NUM_X_GRIDS = 10;
    const int NUM_Y_GRIDS = 10;

    std::array<cRappPointCloud::vCloud_t, NUM_X_GRIDS * NUM_Y_GRIDS> grid_data;

    std::array<int, NUM_X_GRIDS> x_bounds;
    std::array<int, NUM_Y_GRIDS> y_bounds;

    auto dx = (pc.maxX_mm() - pc.minX_mm()) / NUM_X_GRIDS;
    auto dy = (pc.maxY_mm() - pc.minY_mm()) / NUM_Y_GRIDS;

    for (int i = 0; i < NUM_X_GRIDS; ++i)
    {
        x_bounds[i] = pc.minX_mm() + i * dx;
    }

    for (int i = 0; i < NUM_Y_GRIDS; ++i)
    {
        y_bounds[i] = pc.minY_mm() + i * dy;
    }

    for (auto point : pc)
    {
        int i = 0;
        for (; i < NUM_X_GRIDS; ++i)
        {
            if (point.x_mm < x_bounds[i])
            {
                break;
            }
        }
        --i;

        int j = 0;
        for (; j < NUM_Y_GRIDS; ++j)
        {
            if (point.y_mm < y_bounds[j])
            {
                break;
            }
        }
        --j;

        int n = i * NUM_Y_GRIDS + j;

        grid_data[n].push_back(point);
    }

    cRappPointCloud::vCloud_t data;

    for (auto block : grid_data)
    {
        int minZ_mm = std::numeric_limits<int>::max();
        int maxZ_mm = std::numeric_limits<int>::min();

        for (auto& point : block)
        {
            minZ_mm = std::min(minZ_mm, point.z_mm);
            maxZ_mm = std::max(maxZ_mm, point.z_mm);
        }

        int32_t z_threshold = threshold(minZ_mm, maxZ_mm, threshold_pct);

        auto end_it = std::remove_if(block.begin(), block.end(), [z_threshold](rfm::sPoint3D_t a)
            {
                return (a.z_mm > z_threshold) || (a.h_mm == rfm::INVALID_HEIGHT);
            });

        block.erase(end_it, block.end());

        data.insert(data.end(), block.begin(), block.end());
    }

    if (data.size() < MIN_SET_OF_POINTS)
        return sPitchAndRoll_t();

    // Compute the centroid of all of the points...
    rfm::sCentroid_t centroid = computeCentroid(data);

    cRappPointCloud temp(centroid, data);

    return fitPointCloudToGroundMesh(temp);
}

void shiftPointCloudToAGL(cRappPointCloud& pc)
{
    for (auto& point : pc)
    {
        if (point.h_mm != rfm::INVALID_HEIGHT)
        {
            point.z_mm -= point.h_mm;
            point.h_mm = 0;
        }
        else
        {
            point.z_mm;
            point.h_mm = 0;
        }
    }

    pc.recomputeBounds();
}

void shiftPointCloudToAGL(cRappPointCloud::vCloud_t& pc)
{
    for (auto& point : pc)
    {
        if (point.h_mm != rfm::INVALID_HEIGHT)
        {
            point.z_mm -= point.h_mm;
            point.h_mm = 0;
        }
        else
        {
            point.z_mm;
            point.h_mm = 0;
        }
    }
}
