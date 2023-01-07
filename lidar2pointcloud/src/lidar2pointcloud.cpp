
#include "BS_thread_pool.hpp"

#include <ouster/simple_blas.h>

#include <lyra/lyra.hpp>

#include <eigen3/Eigen/Eigen>

#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <numbers>

ouster::cRotationMatrix<double> setSensorOrientation(double yaw_deg, double pitch_deg, double roll_deg)
{
	auto pitch = pitch_deg * std::numbers::pi / 180.0;
	auto roll = roll_deg * std::numbers::pi / 180.0;
	auto yaw = yaw_deg * std::numbers::pi / 180.0;

	ouster::cRotationMatrix<double> rotation;

	auto c1 = rotation.column(0);
	c1[0] = cos(yaw) * cos(pitch);
	c1[1] = sin(yaw) * cos(pitch);
	c1[2] = -sin(pitch);

	auto c2 = rotation.column(1);
	c2[0] = cos(yaw) * sin(pitch) * sin(roll) - sin(yaw) * cos(roll);
	c2[1] = sin(yaw) * sin(pitch) * sin(roll) + cos(yaw) * cos(roll);
	c2[2] = cos(pitch) * sin(roll);

	auto c3 = rotation.column(2);
	c3[0] = cos(yaw) * sin(pitch) * cos(roll) + sin(yaw) * sin(roll);
	c3[1] = sin(yaw) * sin(pitch) * cos(roll) - cos(yaw) * sin(roll);
	c3[2] = cos(pitch) * cos(roll);

	return rotation;
}


std::mutex g_console_mutex;


void console_message(const std::string& msg)
{
	std::lock_guard<std::mutex> guard(g_console_mutex);
	std::cout << msg << std::endl;
}


int main(int argc, char** argv)
{
	using namespace std::filesystem;

	double pitch_deg = 0.0;
	double roll_deg = 0.0;
	double yaw_deg = 0.0;

	double min_dist_m = 0;
	double max_dist_m = 1000;

	int num_of_threads = 0;
	std::string input_directory = current_path().string();

	auto cli = lyra::cli()
		| lyra::opt(pitch_deg, "pitch (deg)")
		["-p"]["--pitch_deg"]
		("The pitch angle of the sensor with respect to the horizontal.  Range is +90 to -90 degrees.")
		| lyra::opt(roll_deg, "roll (deg)")
		["-r"]["--roll_deg"]
		("The roll angle of the sensor with respect to the horizontal.  Range is +180 to -180 degrees.")
		| lyra::opt(yaw_deg, "yaw (deg)")
		["-y"]["--yaw_deg"]
		("The yaw angle of the sensor with respect to the east axis.  Range is 0 to 360 degrees.")
		| lyra::opt(min_dist_m, "min distance (m)")
		["-m"]["--min_distance_m"]
		("The minimum distance of the lidar return to use in meters.")
		| lyra::opt(max_dist_m, "max distance (m)")
		["-x"]["--max_distance_m"]
		("The maximum distance of the lidar return to use in meters.")
		| lyra::opt(num_of_threads, "threads")
		["-t"]["--threads"]
		("The number of threads to use for repairing data files.")
		.optional()
		| lyra::arg(input_directory, "input directory")
		("The path to input directory for repairing of ceres data.");

	auto result = cli.parse({argc, argv});

	if (!result)
	{
		std::cerr << "Error in command line: " << result.message() << std::endl;
		return 1;
	}

	auto refMatrix = setSensorOrientation(yaw_deg, pitch_deg, roll_deg);

	auto pitch = pitch_deg * std::numbers::pi / 180.0;
	auto roll = roll_deg * std::numbers::pi / 180.0;
	auto yaw = yaw_deg * std::numbers::pi / 180.0;

//	Eigen::AngleAxisd rollAngle(roll, Eigen::Vector3d::UnitZ());
//	Eigen::AngleAxisd yawAngle(yaw, Eigen::Vector3d::UnitY());
//	Eigen::AngleAxisd pitchAngle(pitch, Eigen::Vector3d::UnitX());

	Eigen::AngleAxisd rollAngle(roll, Eigen::Vector3d::UnitX());
	Eigen::AngleAxisd yawAngle(yaw, Eigen::Vector3d::UnitZ());
	Eigen::AngleAxisd pitchAngle(pitch, Eigen::Vector3d::UnitY());

	//	Eigen::Quaternion<double> q = rollAngle * yawAngle * pitchAngle;
	Eigen::Quaternion<double> q = yawAngle * pitchAngle * rollAngle;

	Eigen::Matrix3d rotationMatrix = q.matrix();
	auto c1 = rotationMatrix.col(0);
	auto c11 = c1[0];
	auto c12 = c1[1];
	auto c13 = c1[2];
	auto c2 = rotationMatrix.col(1);
	auto c21 = c2[0];
	auto c22 = c2[1];
	auto c23 = c2[2];
	auto c3 = rotationMatrix.col(2);
	auto c31 = c3[0];
	auto c32 = c3[1];
	auto c33 = c3[2];

/*
	const std::filesystem::path input{ input_directory };

	int max_threads = std::thread::hardware_concurrency();

	num_of_threads = std::max(num_of_threads, 0);
	num_of_threads = std::min(num_of_threads, max_threads);

	// Constructs a thread pool with as many threads as available in the hardware.
	BS::thread_pool pool(num_of_threads);
	int n = pool.get_thread_count();

	std::cout << "Using " << n << " threads of a possible " << max_threads << std::endl;
*/

	return 0;
}

