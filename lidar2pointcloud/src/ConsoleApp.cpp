
#include "BS_thread_pool.hpp"

#include "lidar2pointcloud.hpp"
#include "FileProcessor.hpp"
#include "StringUtils.hpp"

#include "Kinematics_Constant.hpp"
#include "Kinematics_Dolly.hpp"
#include "Kinematics_GPS.hpp"
#include "Kinematics_SLAM.hpp"

#include <lyra/lyra.hpp>

#include <eigen3/Eigen/Eigen>

#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <numbers>


std::mutex g_console_mutex;


void console_message(const std::string& msg)
{
	std::lock_guard<std::mutex> guard(g_console_mutex);
	std::cout << msg << std::endl;
}


int main(int argc, char** argv)
{
	using namespace std::filesystem;
	using namespace nStringUtils;


	double pitch_deg = 0.0;
	double roll_deg = 0.0;
	double yaw_deg = 0.0;

	double min_dist_m = 0;
	double max_dist_m = 1000;

	bool aggregatePointCloud = false;
	bool saveReducedPointCloud = false;
	std::string kinematics;
	bool isFile = false;
	bool showHelp = false;
	std::string config_file;

	int num_of_threads = 1;
	std::string input_directory = current_path().string();
	std::string output_directory = current_path().string();

	auto cli = lyra::cli()
		| lyra::help(showHelp)
		("Show usage information.")
		| lyra::opt(config_file, "configuration")
		["-c"]["--config"]
		("Configuration file to set the options in generating the point cloud data.")
		.optional()
		| lyra::opt(isFile)
		["-f"]["--file"]
		("Operate on a single file instead of directory.")
		| lyra::opt(aggregatePointCloud)
		["-a"]["--aggregate"]
		("Aggregate lidar data into a single pointcloud.")
		| lyra::opt(kinematics, "kinematics type")
		["-k"]["--kinematic"]
		("Specify the type of kinematics to apply to the pointcloud data.")
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
		["-s"]["--space"]
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

	if (aggregatePointCloud)
	{
		cLidar2PointCloud::saveAggregatePointCloud();
	}
	else if (saveReducedPointCloud)
	{
		cLidar2PointCloud::saveReducedPointCloud();
	}

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

		if (dir_entry.path().extension() != ".ceres")
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

			if (dir_entry.path().extension() != ".ceres")
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


	eKinematics model;
	model = eKinematics::NONE;

	double eastSpeed_mmps  = 0;
	double northSpeed_mmps = 0;
	double vertSpeed_mmps  = 0;

	if (icontains(kinematics, "constant"))
	{
		model = eKinematics::CONSTANT;

		auto parameters = get_parameters(kinematics);
		if (parameters.size() != 3)
		{
			std::cout << cli << std::endl;
			return 0;
		}

		eastSpeed_mmps  = std::stod(parameters[0]);
		northSpeed_mmps = std::stod(parameters[1]);
		vertSpeed_mmps  = std::stod(parameters[2]);
	}
	else if (iequal(kinematics, "slam"))
	{
		model = eKinematics::DOLLY;
	}
	else if (iequal(kinematics, "dolly"))
	{
		model = eKinematics::DOLLY;
	}
	else if (iequal(kinematics, "gps"))
	{
		model = eKinematics::GPS;
	}

	/*
	 * Add all of the files to process to the thread pool
	 */
	for (auto& in_file : files_to_process)
	{
		std::filesystem::path out_file;
		auto fe = removeProcessedTimestamp(in_file.path().filename().string());

		if (isFile)
		{
			out_file = std::filesystem::path{ output_directory };
		}

		if (out_file.empty())
		{
			std::string out_filename = fe.filename;
			out_file = output_directory;
			out_file /= addProcessedTimestamp(out_filename);

			if (!fe.extension.empty())
			{
				out_file += ".";
				out_file += fe.extension;
				//				out_file.replace_extension(fe.extension);
			}
		}

		cFileProcessor* fp = new cFileProcessor(in_file, out_file);

		std::unique_ptr<cKinematics> kinematics;

		switch (model)
		{
		case eKinematics::CONSTANT:
			kinematics = std::make_unique<cKinematics_Constant>(eastSpeed_mmps,
				northSpeed_mmps, vertSpeed_mmps);
			break;
		case eKinematics::DOLLY:
			kinematics = std::make_unique<cKinematics_Dolly>();
			break;
		case eKinematics::GPS:
			kinematics = std::make_unique<cKinematics_GPS>();
			break;
		case eKinematics::SLAM:
			kinematics = std::make_unique<cKinematics_SLAM>();
			break;
		}

		kinematics->rotateToSEU(true);
		kinematics->setSensorOrientation(yaw_deg, pitch_deg, roll_deg);
		fp->setKinematicModel(std::move(kinematics));

		pool.push_task(&cFileProcessor::process_file, fp);

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

