
#include "FieldUtils.hpp"

#include <dlib/optimization.h>

#include <vtkDelaunay2D.h>
#include <vtkDoubleArray.h>
#include <vtkTable.h>
#include <vtkTableToPolyData.h>

#include <array>

#include "Constants.hpp"

namespace
{
    constexpr std::size_t MIN_SET_OF_POINTS = 10;
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
typedef dlib::matrix<double, 1, 1> input_one_variable;
typedef dlib::matrix<double, 2, 1> slope_interslope_vector;


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


