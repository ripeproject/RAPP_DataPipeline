
#include "BS_thread_pool.hpp"

#include "LidarStatistics.hpp"
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

	int num_of_threads = 0;
	std::string input_directory = current_path().string();
	std::string output_directory;

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
		("The number of threads to use for analyzing data files.")
		.optional()
		| lyra::arg(input_directory, "input directory")
		("The path to input directory for analyzing lidar data.")
		| lyra::arg(output_directory, "output directory")
		("The path of output directory for storing lidar statistics.");

	auto result = cli.parse({argc, argv});

	if (!result)
	{
		std::cerr << "Error in command line: " << result.message() << std::endl;
		return 1;
	}

	cLidarStatistics::setValidRange_m(min_dist_m, max_dist_m);
	cLidarStatistics::setSensorOrientation(yaw_deg, pitch_deg, roll_deg);

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

	std::filesystem::path out_dir;

	if (output_directory.empty())
	{
		out_dir = input_directory;
		out_dir /= "statistics";
	}
	else
	{
		out_dir = output_directory;
	}

	if (!std::filesystem::exists(out_dir))
	{
		if (!std::filesystem::create_directory(out_dir))
		{
			return -1;
		}
	}

	std::vector<cFileProcessor*> file_processors;

	for (auto& in_file : files_to_process)
	{
		auto fe = removeTimestamp(in_file.path().filename().string());

		std::string out_filename = fe.filename;
		std::filesystem::path out_file = out_dir;
		out_file /= out_filename;

		cFileProcessor* fp = new cFileProcessor();

		pool.push_task(&cFileProcessor::process_file, fp, in_file, out_file);

		file_processors.push_back(fp);

		break;
	}

	files_to_process.clear();

	pool.wait_for_tasks();

	for (auto file_processor : file_processors)
	{
		delete file_processor;
	}

	return 0;
}

