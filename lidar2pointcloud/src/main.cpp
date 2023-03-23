
#include "BS_thread_pool.hpp"

#include "lidar2pointcloud.hpp"
#include "FileProcessor.hpp"

#include <lyra/lyra.hpp>

#include <eigen3/Eigen/Eigen>

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
		auto ext = filename.find_last_of('.');
		if (ext != std::string::npos)
		{
			base = filename.substr(0, ext);
			extension = filename.substr(ext + 1);
			return { base, extension };
		}

		base = filename;
		return { base, extension };
	}

	auto ext = filename.find_last_of('.');
	if (ext != std::string::npos)
	{
		 extension = filename.substr(ext + 1);
	}

	return {base, extension};
}

std::string add_timestamp(std::string filename)
{
	char timestamp[100] = { '\0' };

	std::time_t t = std::time(nullptr);
	std::strftime(timestamp, sizeof(timestamp), "%Y%m%d%H%M%S", std::localtime(&t));

	filename += "_";
	filename += timestamp;

	return filename;
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

	bool saveReducedPointCloud = false;
	bool useSLAM = false;
	bool isFile = false;
	bool showHelp = false;

	int num_of_threads = 0;
	std::string input_directory = current_path().string();
	std::string output_directory = current_path().string();

	auto cli = lyra::cli()
		| lyra::help(showHelp)
		("Show usage information.")
		| lyra::opt(isFile)
		["-f"]["--file"]
		("Operate on a single file instead of directory.")
		| lyra::opt(useSLAM)
		["-s"]["--slam"]
		("Use SLAM to create a single pointcloud.")
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
		| lyra::opt(saveReducedPointCloud)
		["-c"]["--compact"]
		("Save a reduced pointcloud.  Points outside min/max distances are not stored.")
		| lyra::opt(num_of_threads, "threads")
		["-t"]["--threads"]
		("The number of threads to use for repairing data files.")
		.optional()
		| lyra::arg(input_directory, "input directory")
		("The path to input directory/file for converting lidar to pointcloud data.")
		.required()
		| lyra::arg(output_directory, "output directory")
		("The path to output directory/file for converted pointcloud data.")
		.required();

	auto result = cli.parse({argc, argv});

	if (showHelp)
	{
		std::cout << cli << std::endl;
		return 0;
	}

	if (!result)
	{
		std::cerr << "Error in command line: " << result.message() << std::endl;
		std::cerr << std::endl;
		std::cerr << cli << std::endl;
		return 1;
	}

	cLidar2PointCloud::setValidRange_m(min_dist_m, max_dist_m);
	cLidar2PointCloud::setSensorOrientation(yaw_deg, pitch_deg, roll_deg);

	if (saveReducedPointCloud)
	{
		cLidar2PointCloud::saveReducedPointCloud();
	}

/*
	pitch_deg = -90.0;
	roll_deg = 10.0;
	yaw_deg = 60.0;

	auto pitch = pitch_deg * std::numbers::pi / 180.0;
	auto roll = roll_deg * std::numbers::pi / 180.0;
	auto yaw = yaw_deg * std::numbers::pi / 180.0;

	Eigen::Vector3d imu(0,0,-1);

	Eigen::AngleAxisd rollAngle1(roll, Eigen::Vector3d::UnitX());
	Eigen::AngleAxisd rollAngle2(-roll, Eigen::Vector3d::UnitX());
	Eigen::AngleAxisd yawAngle1(yaw, Eigen::Vector3d::UnitZ());
	Eigen::AngleAxisd yawAngle2(-yaw, Eigen::Vector3d::UnitZ());
	Eigen::AngleAxisd pitchAngle1(pitch, Eigen::Vector3d::UnitY());
	Eigen::AngleAxisd pitchAngle2(-pitch, Eigen::Vector3d::UnitY());

	Eigen::Quaternion<double> q1 = yawAngle1 * pitchAngle1 * rollAngle1;
	Eigen::Matrix3d rotationMatrix1 = q1.matrix();

	std::cout << "IMU" << std::endl;
	std::cout << imu[0] << ", " << imu[1] << ", " << imu[2] << std::endl;

	std::cout << std::endl;
	std::cout << "Yaw, Pitch, Roll" << std::endl;
	auto r1_0 = rotationMatrix1.row(0);
	std::cout << r1_0[0] << ", " << r1_0[1] << ", " << r1_0[2] << std::endl;

	auto r1_1 = rotationMatrix1.row(1);
	std::cout << r1_1[0] << ", " << r1_1[1] << ", " << r1_1[2] << std::endl;

	auto r1_2 = rotationMatrix1.row(2);
	std::cout << r1_2[0] << ", " << r1_2[1] << ", " << r1_2[2] << std::endl;

	imu = rotationMatrix1 * imu;

	std::cout << std::endl;
	std::cout << "IMU" << std::endl;
	std::cout << imu[0] << ", " << imu[1] << ", " << imu[2] << std::endl;

	Eigen::Quaternion<double> q2 = rollAngle2 * pitchAngle2 * yawAngle2;
	Eigen::Matrix3d rotationMatrix2 = q2.matrix();

	std::cout << std::endl;
	std::cout << "Roll, Pitch, Yaw" << std::endl;

	auto r2_0 = rotationMatrix2.row(0);
	std::cout << r2_0[0] << ", " << r2_0[1] << ", " << r2_0[2] << std::endl;

	auto r2_1 = rotationMatrix2.row(1);
	std::cout << r2_1[0] << ", " << r2_1[1] << ", " << r2_1[2] << std::endl;

	auto r2_2 = rotationMatrix2.row(2);
	std::cout << r2_2[0] << ", " << r2_2[1] << ", " << r2_2[2] << std::endl;

	imu = rotationMatrix2 * imu;

	std::cout << std::endl;
	std::cout << "IMU" << std::endl;
	std::cout << imu[0] << ", " << imu[1] << ", " << imu[2] << std::endl;

	return 0;
*/

	const std::filesystem::path input{ input_directory };

	std::vector<directory_entry> files_to_process;

	/*
	 * Create list of files to process
	 */
	if (isFile)
	{
		auto dir_entry = std::filesystem::directory_entry{ input };

		if (!dir_entry.is_regular_file())
			return 2;

		if (dir_entry.path().extension() == "ceres")
			return 3;

		if (!isCeresFile(dir_entry.path().string()))
			return 4;

		files_to_process.push_back(dir_entry);
	}
	else
	{
		// Scan input directory for all CERES files to operate on
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
	}

	int max_threads = std::thread::hardware_concurrency();

	num_of_threads = std::max(num_of_threads, 0);
	num_of_threads = std::min(num_of_threads, max_threads);

	// Constructs a thread pool with as many threads as available in the hardware.
	BS::thread_pool pool(num_of_threads);
	int n = pool.get_thread_count();

	std::cout << "Using " << n << " threads of a possible " << max_threads << std::endl;

	std::vector<cFileProcessor*> file_processors;

	/*
	 * Make sure the output directory exists
	 */
	const std::filesystem::path output{ output_directory };

	std::filesystem::directory_entry output_dir;

	if (isFile && output.has_extension())
	{
		output_dir = std::filesystem::directory_entry{ output.parent_path() };
	}
	else
	{
		output_dir = std::filesystem::directory_entry{ output };
	}

	if (!output_dir.exists())
	{
		std::filesystem::create_directories(output_dir);
	}

	/*
	 * Add all of the files to process to the thread pool
	 */
	for (auto& in_file : files_to_process)
	{
		std::filesystem::path out_file;
		auto fe = removeTimestamp(in_file.path().filename().string());

		if (isFile)
		{
			out_file = std::filesystem::path{ output_directory };
		}

		if (out_file.empty())
		{
			std::string out_filename = fe.filename;
			out_file = output_directory;
			out_file /= add_timestamp(out_filename);

			if (!fe.extension.empty())
			{
				out_file.replace_extension(fe.extension);
			}
		}

		cFileProcessor* fp = new cFileProcessor();

		pool.push_task(&cFileProcessor::process_file, fp, in_file, out_file);

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

