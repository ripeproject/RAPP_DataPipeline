
#include "BS_thread_pool.hpp"

#include "lidar2pointcloud.hpp"
#include "FileProcessor.hpp"

#include <lyra/lyra.hpp>

#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <numbers>

struct sFilenameAndExtension
{
	std::string filename;
	std::string extension;
};

sFilenameAndExtension removeTimestamp(const std::string& filename)
{
	std::string base;
	std::string extension;

	auto dash = filename.find_last_of('_');
	if (dash != std::string::npos)
	{
		base = filename.substr(0, dash);
	}
	else
	{

	}

	auto ext = filename.find_last_of('.');
	if (ext != std::string::npos)
	{
		 extension = filename.substr(ext + 1);
	}

	return {base, extension};
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
	std::string output_directory = current_path().string();

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
		("The path to input directory for converting lidar to pointcloud data.")
		| lyra::arg(output_directory, "output directory")
		("The path to output directory for converting lidar to pointcloud data.");

	auto result = cli.parse({argc, argv});

	if (!result)
	{
		std::cerr << "Error in command line: " << result.message() << std::endl;
		return 1;
	}

	cLidar2PointCloud::setValidRange_m(min_dist_m, max_dist_m);
	cLidar2PointCloud::setSensorOrientation(yaw_deg, pitch_deg, roll_deg);

/*
	auto pitch = pitch_deg * std::numbers::pi / 180.0;
	auto roll = roll_deg * std::numbers::pi / 180.0;
	auto yaw = yaw_deg * std::numbers::pi / 180.0;

	Eigen::AngleAxisd rollAngle(roll, Eigen::Vector3d::UnitX());
	Eigen::AngleAxisd yawAngle(yaw, Eigen::Vector3d::UnitZ());
	Eigen::AngleAxisd pitchAngle(pitch, Eigen::Vector3d::UnitY());

	Eigen::Quaternion<double> q = yawAngle * pitchAngle * rollAngle;
	Eigen::Matrix3d rotationMatrix = q.matrix();
*/


	const std::filesystem::path input{ input_directory };

	std::vector<directory_entry> files_to_process;
	for (auto const& dir_entry : std::filesystem::directory_iterator{ input })
	{
		if (!dir_entry.is_regular_file())
			continue;

		if (dir_entry.path().extension() == "ceres")
			continue;

		if (!isCeresFile(dir_entry.path().string()))
			continue;

		files_to_process.push_back(dir_entry);
	}

	int max_threads = std::thread::hardware_concurrency();

	num_of_threads = std::max(num_of_threads, 0);
	num_of_threads = std::min(num_of_threads, max_threads);

	// Constructs a thread pool with as many threads as available in the hardware.
	BS::thread_pool pool(num_of_threads);
	int n = pool.get_thread_count();

	std::cout << "Using " << n << " threads of a possible " << max_threads << std::endl;

	std::vector<cFileProcessor*> file_processors;

	for (auto& file : files_to_process)
	{
		std::filesystem::path file_path = file;
		auto fe = removeTimestamp(file_path.filename().string());

		cFileProcessor* fp = new cFileProcessor();

//		pool.push_task(&cFileProcessor::process_file, fp, file);

		file_processors.push_back(fp);
	}

	files_to_process.clear();

	pool.wait_for_tasks();

	for (auto file_processor : file_processors)
	{
		delete file_processor;
	}

	return 0;
}

