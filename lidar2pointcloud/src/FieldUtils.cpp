
#include "FieldUtils.hpp"


#include <eigen3/Eigen/Eigen>

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

extern void update_progress(const int id, const int progress_pct);

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

    double deltaAngle(double angle1_deg, double angle2_deg)
    {
        double delta = angle1_deg - angle2_deg;

        if (std::abs(delta) > 180.0)
        {
            if (delta > 0.0)
            {
                delta = 360.0 - delta;
            }
            else
            {

            }
        }

        return delta;
    }

    template<typename T>
    inline void _rotate(T& x, T& y, T& z, const Eigen::Matrix3d& r)
    {
        const auto& rX = r.col(0);
        const auto& rY = r.col(1);
        const auto& rZ = r.col(2);

        double a = x * rX[0] + y * rX[1] + z * rX[2];
        double b = x * rY[0] + y * rY[1] + z * rY[2];
        double c = x * rZ[0] + y * rZ[1] + z * rZ[2];

        x = static_cast<T>(a);
        y = static_cast<T>(b);
        z = static_cast<T>(c);
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

    return data;
}

//-----------------------------------------------------------------------------
typedef dlib::matrix<double, 4, 1> input_vector;
typedef dlib::matrix<double, 1, 1> input_one_variable;
typedef dlib::matrix<double, 2, 1> input_two_variable;
typedef dlib::matrix<double, 3, 1> input_three_variables;
typedef dlib::matrix<double, 2, 1> slope_interslope_vector;
typedef dlib::matrix<double, 2, 1> pitch_roll_vector;
typedef dlib::matrix<double, 1, 1> pitch_vector;
typedef dlib::matrix<double, 1, 1> roll_vector;

constexpr size_t NUM_HEIGHT_INTERP_POINTS = 10;
typedef dlib::matrix<double, 2*NUM_HEIGHT_INTERP_POINTS, 1> height_interp_table_vector;

constexpr size_t NUM_ANGLE_INTERP_POINTS = 10;
typedef dlib::matrix<double, 3* NUM_ANGLE_INTERP_POINTS, 1> angle_interp_table_vector;


double modelLine(const input_one_variable& input, const slope_interslope_vector& params)
{
    const double m = params(0);
    const double b = params(1);

    const double x = input(0);

    double c = m * x + b;

    return c;
}

double residualLine(const std::pair<input_one_variable, double>& data, const slope_interslope_vector& params)
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

    slope_interslope_vector line;
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

    slope_interslope_vector line;
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

//-----------------------------------------------------------------------------
double modelHeightTable(const input_one_variable& input, const height_interp_table_vector& params)
{
    const double x = input(0);

    double d0 = params(0);
    double h0 = params(1);

    double d1 = d0;
    double h1 = h0;

    std::size_t i = 2;
    std::size_t n = params.size();
    for (; i < n; i += 2)
    {
        d1 = params(i);
        h1 = params(i + 1);

        if (x <= d1)
            break;

        d0 = d1;
        h0 = h1;
    }

    if (i >= n)
    {
        d0 = params(n - 4);
        h0 = params(n - 3);
        d1 = params(n - 2);
        h1 = params(n - 1);
    }

    const double d = (d1 - d0);

    const double m = (h1 - h0) / d;

    // Shift the x value to be inside the interval d0 to d1
    double h = m * (x - d0) + h0;

    return h;
}

double residualHeightTable(const std::pair<input_one_variable, double>& data, const height_interp_table_vector& params)
{
    auto r = modelHeightTable(data.first, params) - data.second;

    if (std::isnan(r))
    {
        r = 0.0;
    }

    return r;
}

height_interp_table_vector residualHeightTable_derivative(const std::pair<input_one_variable, double>& data, const height_interp_table_vector& params)
{
    const double delta_h = 0.0001;

    height_interp_table_vector derivative;
    derivative = 0.00000000001;
    derivative(0) = 0;

    const double x = data.first(0);

    std::size_t i = 0;
    double d0 = params(i);
    double h0 = params(i+1);
    double d1 = d0;
    double h1 = h0;

    std::size_t n = params.size();
    for (i = 2; i < n; i += 2)
    {
        d1 = params(i);
        h1 = params(i + 1);

        if (x <= d1)
            break;

        d0 = d1;
        h0 = h1;
    }

    if (i >= n)
    {
        d0 = params(n - 4);
        h0 = params(n - 3);
        d1 = params(n - 2);
        h1 = params(n - 1);
        i = n - 2;
    }

    const double delta_d = (d1 - d0);

    const double m  = (h1 - h0) / delta_d;
    const double m0 = (h1 - (h0 + delta_h)) / delta_d;
    const double m1 = ((h1 + delta_h) - h0) / delta_d;

    const double y  = m  * (x - d0) + h0;
    const double y0 = m0 * (x - d0) + (h0 + delta_h);
    const double y1 = m1 * (x - d0) + h0;

    const auto r  = y  - data.second;
    const auto r0 = y0 - data.second;
    const auto r1 = y1 - data.second;

    auto delta_r0 = (r0 - r) / delta_h;
    auto delta_r1 = (r1 - r) / delta_h;

    derivative(i - 1) = delta_r0;
    derivative(i) = 0;
    derivative(i + 1) = delta_r1;

    return derivative;
}

//sOffsetInterpTable_t fitPointCloudHeightTable(std::vector<rfm::sPointCloudTranslationInterpPoint_t> heights, uint16_t steps)
sOffsetInterpTable_t fitPointCloudHeightTable(std::vector<rfm::sPointCloudTranslationInterpPoint_t> heights)
{
    using namespace dlib;

    if (heights.empty())
    {
        return sOffsetInterpTable_t();
    }

    std::sort(heights.begin(), heights.end(), [](rfm::sPointCloudTranslationInterpPoint_t a, rfm::sPointCloudTranslationInterpPoint_t b)
        {
            return (a.displacement_m < b.displacement_m);
        });

    double d0 = heights.front().displacement_m;
    double d1 = heights.back().displacement_m;

    double delta = 0.0;
    
    if (NUM_HEIGHT_INTERP_POINTS > 1)
        delta = (d1 - d0) / (NUM_HEIGHT_INTERP_POINTS - 1);
    else
        delta = (d1 - d0);

    height_interp_table_vector table;
    table = 1;

    std::size_t n = table.size();
    for (std::size_t i = 0; i < n; i += 2)
    {
        table(i) = d0;
        d0 += delta;
    }

    // Make sure the last displacement entry is the last height displacement
    table(n - 2) = d1;

    std::vector<std::pair<input_one_variable, double>> samples;

    input_one_variable input;

    for (auto& point : heights)
    {
        input(0) = point.displacement_m;

        const double output = point.height_m;

        // save the pair
        samples.push_back(std::make_pair(input, output));
    }

    // Before we do anything, let's make sure that our derivative function defined above matches
    // the approximate derivative computed using central differences (via derivative()).  
    // If this value is big then it means we probably typed the derivative function incorrectly.
    auto R2 = solve_least_squares_lm(objective_delta_stop_strategy(0.001, 50), //.be_verbose(),
        residualHeightTable,
        residualHeightTable_derivative,
        samples,
        table);

    sOffsetInterpTable_t result;

    for (std::size_t i = 0; i < n; i += 2)
    {
        rfm::sPointCloudTranslationInterpPoint_t point;
        point.displacement_m = table(i);
        point.height_m = table(i + 1);
        result.offset_m.push_back(point);
    }

    input(0) = heights.front().displacement_m;
    auto h0 = modelHeightTable(input, table);

    input(0) = heights.back().displacement_m;
    auto h1 = modelHeightTable(input, table);

    result.offset_m.front().displacement_m = heights.front().displacement_m;
    result.offset_m.front().height_m = h0;
    result.offset_m.back().displacement_m  = heights.back().displacement_m;
    result.offset_m.back().height_m = h1;

    result.R = sqrt(R2);

    result.valid = true;

    return result;
}

sOffsetInterpTable_t computePointCloudHeightTable(std::vector<rfm::sPointCloudTranslationInterpPoint_t> heights, uint16_t steps)
{
    if (heights.empty())
    {
        return sOffsetInterpTable_t();
    }

    std::sort(heights.begin(), heights.end(), [](rfm::sPointCloudTranslationInterpPoint_t a, rfm::sPointCloudTranslationInterpPoint_t b)
        {
            return (a.displacement_m < b.displacement_m);
        });

    double d0 = heights.front().displacement_m;
    double d1 = heights.back().displacement_m;

    double delta = 0.0;

    if (steps > 1)
        delta = (d1 - d0) / (steps - 1);
    else
        delta = (d1 - d0);

    sOffsetInterpTable_t result;

    // Use 50% of point in the front of the samples
    {
        double lv = d0;
        double uv = d0 + (0.5 * delta);

        auto lb = std::lower_bound(heights.begin(), heights.end(), lv, [](const auto x, double bound)
            {
                return x.displacement_m < bound;
            }
        );

        auto ub = std::upper_bound(heights.begin(), heights.end(), uv, [](double bound, const auto x)
            {
                return bound < x.displacement_m;
            }
        );

        double avg_height = 0.0;
        size_t n = 0;

        for (; lb != ub; ++lb)
        {
            avg_height += lb->height_m;
            ++n;
        }

        rfm::sPointCloudTranslationInterpPoint_t point;
        point.displacement_m = d0;

        if (n > 0)
            point.height_m = (avg_height / n);
        else
            point.height_m = avg_height;

        result.offset_m.push_back(point);

        d0 += delta;
    }

    // Use the samples from 25% behind and 25% ahead to compute of height at the given point
    for (std::size_t i = 1; i < (steps-1); ++i)
    {
        double lv = d0 - (0.25 * delta);
        double uv = d0 + (0.25 * delta);

        auto lb = std::lower_bound(heights.begin(), heights.end(), lv, [](const auto x, double bound)
            {
                return x.displacement_m < bound;
            }
        );

        auto ub = std::upper_bound(heights.begin(), heights.end(), uv, [](double bound, const auto x)
            {
                return bound < x.displacement_m;
            }
        );

        double avg_height = 0.0;
        size_t n = 0;

        for (; lb != ub; ++lb)
        {
            avg_height += lb->height_m;
            ++n;
        }

        rfm::sPointCloudTranslationInterpPoint_t point;
        point.displacement_m = d0;

        if (n > 0)
            point.height_m = (avg_height / n);
        else
            point.height_m = avg_height;

        result.offset_m.push_back(point);

        d0 += delta;
    }

    // Use 50% of point in the end of the samples
    {
        double lv = d0 - (0.5 * delta);
        double uv = d0;

        auto lb = std::lower_bound(heights.begin(), heights.end(), lv, [](const auto x, double bound)
            {
                return x.displacement_m < bound;
            }
        );

        auto ub = std::upper_bound(heights.begin(), heights.end(), uv, [](double bound, const auto x)
            {
                return bound < x.displacement_m;
            }
        );

        double avg_height = 0.0;
        size_t n = 0;

        for (; lb != ub; ++lb)
        {
            avg_height += lb->height_m;
            ++n;
        }

        rfm::sPointCloudTranslationInterpPoint_t point;
        point.displacement_m = d0;

        if (n > 0)
            point.height_m = (avg_height / n);
        else
            point.height_m = avg_height;

        result.offset_m.push_back(point);

        d0 += delta;
    }

    result.R = -1.0;

    result.valid = true;

    return result;
}

//-----------------------------------------------------------------------------

struct sAngles_t
{
    double pitch_deg = 0;
    double roll_deg = 0;
};

sAngles_t modelAngleTable(const input_one_variable& input, const angle_interp_table_vector& params)
{
    const double x = input(0);

    double d0 = params(0);
    double p0 = params(1);
    double r0 = params(2);

    double d1 = d0;
    double p1 = p0;
    double r1 = r0;

    std::size_t i = 3;
    std::size_t n = params.size();
    for (; i < n; i += 3)
    {
        d1 = params(i);
        p1 = params(i + 1);
        r1 = params(i + 2);

        if (x <= d1)
            break;

        d0 = d1;
        p0 = p1;
        r0 = r1;
    }

    if (i >= n)
    {
        d0 = params(n - 7);
        p0 = params(n - 6);
        r0 = params(n - 5);
        d1 = params(n - 3);
        p1 = params(n - 2);
        r1 = params(n - 1);
    }

    const double d = (d1 - d0);

    const double mp = deltaAngle(p1, p0) / d;
    const double mr = deltaAngle(r1, r0) / d;

    double p = mp * (x - d0) + p0;
    double r = mr * (x - d0) + r0;

    return { p, r };
}

double residualAngleTable(const std::pair<input_one_variable, sAngles_t>& data, const angle_interp_table_vector& params)
{
    auto angles = modelAngleTable(data.first, params);
    
    auto r = (angles.pitch_deg - data.second.pitch_deg) + (angles.roll_deg - data.second.roll_deg);

    if (std::isnan(r))
    {
        r = 0.0;
    }

    return r;
}

angle_interp_table_vector residualAngleTable_derivative(const std::pair<input_one_variable, sAngles_t>& data, const angle_interp_table_vector& params)
{
    const double delta_angle = 0.0001;

    angle_interp_table_vector derivative;
    derivative = 0.00000000001;
    derivative(0) = 0.0;

    const double x = data.first(0);

    std::size_t i = 0;
    double d0 = params(i);
    double p0 = params(i + 1);
    double r0 = params(i + 2);

    double d1 = d0;
    double p1 = p0;
    double r1 = r0;

    std::size_t n = params.size();
    for (i = 3; i < n; i += 3)
    {
        d1 = params(i);
        p1 = params(i + 1);
        r1 = params(i + 2);

        if (x <= d1)
            break;

        d0 = d1;
        p0 = p1;
        r0 = r1;
    }

    if (i >= n)
    {
        d0 = params(n - 6);
        p0 = params(n - 5);
        r0 = params(n - 4);
        d1 = params(n - 3);
        p1 = params(n - 2);
        r1 = params(n - 1);
    }

    const double delta_d = (d1 - d0);

    const double mp   = deltaAngle(p1, p0) / delta_d;
    const double mp_0 = deltaAngle(p1, (p0 + delta_angle)) / delta_d;
    const double mp_1 = deltaAngle((p1 + delta_angle), p0) / delta_d;

    const double mr   = deltaAngle(r1, r0) / delta_d;
    const double mr_0 = deltaAngle(r1, (r0 + delta_angle)) / delta_d;
    const double mr_1 = deltaAngle((r1 + delta_angle), r0) / delta_d;

    const double yp   = mp   * (x - d0) + p0;
    const double yp_0 = mp_0 * (x - d0) + (p0 + delta_angle);
    const double yp_1 = mp_1 * (x - d0) + p0;

    const double yr   = mr   * (x - d0) + r0;
    const double yr_0 = mr_0 * (x - d0) + (r0 + delta_angle);
    const double yr_1 = mr_1 * (x - d0) + r0;

    const auto rp  = yp - data.second.pitch_deg;
    const auto rp0 = yp_0 - data.second.pitch_deg;
    const auto rp1 = yp_1 - data.second.pitch_deg;

    const auto rr  = yr - data.second.roll_deg;
    const auto rr0 = yr_0 - data.second.roll_deg;
    const auto rr1 = yr_1 - data.second.roll_deg;

    auto delta_p0 = (rp0 - rp) / delta_angle;
    auto delta_p1 = (rp1 - rp) / delta_angle;

    auto delta_r0 = (rr0 - rr) / delta_angle;
    auto delta_r1 = (rr1 - rr) / delta_angle;

    derivative(i - 2) = delta_p0;
    derivative(i - 1) = delta_r0;
    derivative(i)     = 0.0;
    derivative(i + 1) = delta_p1;
    derivative(i + 2) = delta_r1;

    return derivative;
}

sAnglesInterpTable_t fitPointCloudAngleTable(std::vector<rfm::sPointCloudRotationInterpPoint_t> angles, uint16_t steps)
{
    using namespace dlib;

    if (angles.empty())
    {
        return sAnglesInterpTable_t();
    }

    std::sort(angles.begin(), angles.end(), [](rfm::sPointCloudRotationInterpPoint_t a, rfm::sPointCloudRotationInterpPoint_t b)
        {
            return (a.displacement_m < b.displacement_m);
        });

    angle_interp_table_vector table;
    table = 1;

    double d0 = angles.front().displacement_m;
    double d1 = angles.back().displacement_m;

    double delta = 0.0;

    if (steps > 1)
        delta = (d1 - d0) / (steps - 1);
    else
        delta = (d1 - d0);

    std::size_t n = 3 * steps;
    for (std::size_t i = 0; i < n; i += 3)
    {
        table(i) = d0;
        d0 += delta;
    }

    // Make sure the last displacement entry is the last height displacement
    table(n - 3) = d1;

    std::vector<std::pair<input_one_variable, sAngles_t>> samples;

    input_one_variable input;

    for (auto& point : angles)
    {
        input(0) = point.displacement_m;

        sAngles_t output;
        output.pitch_deg = point.pitch_deg;
        output.roll_deg  = point.roll_deg;

        // save the pair
        samples.push_back(std::make_pair(input, output));
    }

    // Before we do anything, let's make sure that our derivative function defined above matches
    // the approximate derivative computed using central differences (via derivative()).  
    // If this value is big then it means we probably typed the derivative function incorrectly.
    auto R2 = solve_least_squares_lm(objective_delta_stop_strategy(0.001, 50), //.be_verbose(),
        residualAngleTable,
        residualAngleTable_derivative,
        samples,
        table);

    sAnglesInterpTable_t result;

    for (std::size_t i = 0; i < n; i += 3)
    {
        rfm::sPointCloudRotationInterpPoint_t point;
        point.displacement_m = table(i);
        point.pitch_deg = table(i + 1);
        point.roll_deg  = table(i + 2);
        result.angles_deg.push_back(point);
    }

    input(0) = angles.front().displacement_m;
    auto a0 = modelAngleTable(input, table);

    input(0) = angles.back().displacement_m;
    auto a1 = modelAngleTable(input, table);

    result.angles_deg.front().displacement_m = angles.front().displacement_m;
    result.angles_deg.front().pitch_deg = a0.pitch_deg;
    result.angles_deg.front().roll_deg  = a0.roll_deg;

    result.angles_deg.back().displacement_m = angles.back().displacement_m;
    result.angles_deg.back().pitch_deg = a1.pitch_deg;
    result.angles_deg.back().roll_deg  = a1.roll_deg;

    result.R = sqrt(R2);

    result.valid = true;

    return result;
}

sAnglesInterpTable_t computePointCloudAngleTable(std::vector<rfm::sPointCloudRotationInterpPoint_t> angles, uint16_t steps)
{
    using namespace dlib;

    if (angles.empty())
    {
        return sAnglesInterpTable_t();
    }

    std::sort(angles.begin(), angles.end(), [](rfm::sPointCloudRotationInterpPoint_t a, rfm::sPointCloudRotationInterpPoint_t b)
        {
            return (a.displacement_m < b.displacement_m);
        });

    double d0 = angles.front().displacement_m;
    double d1 = angles.back().displacement_m;

    double delta = 0.0;

    if (steps > 1)
        delta = (d1 - d0) / (steps - 1);
    else
        delta = (d1 - d0);

    sAnglesInterpTable_t result;

    // Use 50% of point in the front of the samples
    {
        double lv = d0;
        double uv = d0 + (0.5 * delta);

        auto lb = std::lower_bound(angles.begin(), angles.end(), lv, [](const auto x, double bound)
            {
                return x.displacement_m < bound;
            }
        );

        auto ub = std::upper_bound(angles.begin(), angles.end(), uv, [](double bound, const auto x)
            {
                return bound < x.displacement_m;
            }
        );

        double avg_pitch = 0.0;
        double avg_roll = 0.0;
        size_t n = 0;

        for (; lb != ub; ++lb)
        {
            avg_pitch += lb->pitch_deg;
            avg_roll += lb->roll_deg;
            ++n;
        }

        rfm::sPointCloudRotationInterpPoint_t point;
        point.displacement_m = d0;

        if (n > 0)
        {
            point.pitch_deg = (avg_pitch / n);
            point.roll_deg  = (avg_roll / n);
        }
        else
        {
            point.pitch_deg = avg_pitch;
            point.roll_deg  = avg_roll;
        }

        result.angles_deg.push_back(point);

        d0 += delta;
    }


    for (std::size_t i = 1; i < (steps-1); ++i)
    {
        double lv = d0 - (0.25 * delta);
        double uv = d0 + (0.25 * delta);

        auto lb = std::lower_bound(angles.begin(), angles.end(), lv, [](const auto x, double bound)
            {
                return x.displacement_m < bound;
            }
        );

        auto ub = std::upper_bound(angles.begin(), angles.end(), uv, [](double bound, const auto x)
            {
                return bound < x.displacement_m;
            }
        );

        double avg_pitch = 0.0;
        double avg_roll = 0.0;
        size_t n = 0;

        for (; lb != ub; ++lb)
        {
            avg_pitch += lb->pitch_deg;
            avg_roll += lb->roll_deg;
            ++n;
        }

        rfm::sPointCloudRotationInterpPoint_t point;
        point.displacement_m = d0;

        if (n > 0)
        {
            point.pitch_deg = (avg_pitch / n);
            point.roll_deg = (avg_roll / n);
        }
        else
        {
            point.pitch_deg = avg_pitch;
            point.roll_deg = avg_roll;
        }

        result.angles_deg.push_back(point);

        d0 += delta;
    }

    // Use 50% of point in the back of the samples
    {
        double lv = d0 - (0.5 * delta);
        double uv = d0;

        auto lb = std::lower_bound(angles.begin(), angles.end(), lv, [](const auto x, double bound)
            {
                return x.displacement_m < bound;
            }
        );

        auto ub = std::upper_bound(angles.begin(), angles.end(), uv, [](double bound, const auto x)
            {
                return bound < x.displacement_m;
            }
        );

        double avg_pitch = 0.0;
        double avg_roll = 0.0;
        size_t n = 0;

        for (; lb != ub; ++lb)
        {
            avg_pitch += lb->pitch_deg;
            avg_roll += lb->roll_deg;
            ++n;
        }

        rfm::sPointCloudRotationInterpPoint_t point;
        point.displacement_m = d0;

        if (n > 0)
        {
            point.pitch_deg = (avg_pitch / n);
            point.roll_deg = (avg_roll / n);
        }
        else
        {
            point.pitch_deg = avg_pitch;
            point.roll_deg = avg_roll;
        }

        result.angles_deg.push_back(point);

        d0 += delta;
    }

    result.R = -1.0;

    result.valid = true;

    return result;
}

//-----------------------------------------------------------------------------
double groundheight_pitch_roll(const input_three_variables& input, const pitch_roll_vector& params)
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
    const pitch_roll_vector& params
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
sPitchAndRoll_t fitPointCloudPitchRollToGroundMesh(const cRappPointCloud& pc)
{
    using namespace dlib;

    pitch_roll_vector angles;
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

    return fitPointCloudPitchRollToGroundMesh(temp);
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

    return fitPointCloudPitchRollToGroundMesh(temp);
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

    return { offset, true };
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

    for (const auto& point : pc)
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

    return fitPointCloudPitchRollToGroundMesh(temp);
}

//-----------------------------------------------------------------------------
sPitchAndRoll_t computePcToGroundMeshRotationUsingGrid_deg(const cRappPointCloud::vCloud_t& pc,
    const double minX_mm, const double maxX_mm, const double minY_mm, const double maxY_mm, double threshold_pct)
{
    const int NUM_X_GRIDS = 10;
    const int NUM_Y_GRIDS = 10;

    std::array<cRappPointCloud::vCloud_t, NUM_X_GRIDS* NUM_Y_GRIDS> grid_data;

    std::array<int, NUM_X_GRIDS> x_bounds;
    std::array<int, NUM_Y_GRIDS> y_bounds;

    auto dx = (maxX_mm - minX_mm) / NUM_X_GRIDS;
    auto dy = (maxY_mm - minY_mm) / NUM_Y_GRIDS;

    for (int i = 0; i < NUM_X_GRIDS; ++i)
    {
        x_bounds[i] = minX_mm + i * dx;
    }

    for (int i = 0; i < NUM_Y_GRIDS; ++i)
    {
        y_bounds[i] = minY_mm + i * dy;
    }

    for (const auto& point : pc)
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

    return fitPointCloudPitchRollToGroundMesh(temp);
}

//-----------------------------------------------------------------------------
void shiftPointCloudToAGL(int id, cRappPointCloud& pc)
{
    int i = 0;
    auto n = pc.size();

    for (auto& point : pc)
    {
        if (id >= 0)
            update_progress(id, static_cast<int>((100.0 + i++) / n));

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

void shiftPointCloudToAGL(int id, cRappPointCloud::vCloud_t& pc)
{
    int i = 0;
    auto n = pc.size();

    for (auto& point : pc)
    {
        if (id >= 0)
            update_progress(id, static_cast<int>((100.0 + i++)/n));

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

//-----------------------------------------------------------------------------
void rotate(cRappPointCloud::vCloud_t& pc, double yaw_deg, double pitch_deg, double roll_deg)
{
    double pitch_rad = pitch_deg * nConstants::DEG_TO_RAD;
    double roll_rad = roll_deg * nConstants::DEG_TO_RAD;
    double yaw_rad = yaw_deg * nConstants::DEG_TO_RAD;

    Eigen::AngleAxisd rollAngle(roll_rad, Eigen::Vector3d::UnitX());
    Eigen::AngleAxisd yawAngle(yaw_rad, Eigen::Vector3d::UnitZ());
    Eigen::AngleAxisd pitchAngle(pitch_rad, Eigen::Vector3d::UnitY());

    //	Eigen::Quaternion<double> q = rollAngle * pitchAngle * yawAngle;
    Eigen::Quaternion<double> q = pitchAngle * rollAngle * yawAngle;
    Eigen::Matrix3d rotationMatrix = q.matrix();

    double sum_x = 0.0;
    double sum_y = 0.0;
    double sum_z = 0.0;

    for (const auto& point : pc)
    {
        sum_x += point.x_mm;
        sum_y += point.y_mm;
        sum_z += point.z_mm;
    }

    auto n = pc.size();
    double x_mm = sum_x / n;
    double y_mm = sum_y / n;
    double z_mm = sum_z / n;

    rfm::sCentroid_t centroid = { x_mm , y_mm, z_mm };


    for (auto& point : pc)
    {
        auto x = point.x_mm - centroid.x_mm;
        auto y = point.y_mm - centroid.y_mm;
        auto z = point.z_mm - centroid.z_mm;

        ::_rotate(x, y, z, rotationMatrix);

        point.x_mm = x + centroid.x_mm;
        point.y_mm = y + centroid.y_mm;
        point.z_mm = z + centroid.z_mm;
    }
}

//-----------------------------------------------------------------------------
void translate(cRappPointCloud::vCloud_t& pc, int dx_mm, int dy_mm, int dz_mm)
{
    for (auto& point : pc)
    {
        point.x_mm += dx_mm;
        point.y_mm += dy_mm;
        point.z_mm += dz_mm;
    }
}
