
#include "BS_thread_pool.hpp"

#include "FileProcessor.hpp"

#include "ConfigFileData.hpp"

#include "TextProgressBar.hpp"

#include <lyra/lyra.hpp>

#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <numbers>
#include <memory>
#include <map>


std::mutex g_console_mutex;

namespace
{
	int num_of_threads = 1;
	int numFilesToProcess = 0;
	cTextProgressBar progress_bar;

	std::map<int, int> prev_progress;
}

void console_message(const std::string& msg)
{
	std::lock_guard<std::mutex> guard(g_console_mutex);
	std::cout << "\n" << msg << std::endl;
}

void new_file_progress(const int id, std::string filename)
{
	progress_bar.addProgressEntry(id, filename);
	prev_progress[id] = -1;
}

void update_prefix_progress(const int id, std::string prefix, const int progress_pct)
{
	progress_bar.updateProgressEntry(id, prefix, progress_pct);
	prev_progress[id] = progress_pct;
}

void update_progress(const int id, const int progress_pct)
{
	if (prev_progress[id] != progress_pct)
	{
		progress_bar.updateProgressEntry(id, progress_pct);
		prev_progress[id] = progress_pct;
	}
}

void complete_file_progress(const int id)
{
	progress_bar.finishProgressEntry(id);
	prev_progress.erase(id);
}


int main(int argc, char** argv)
{
	std::cerr << argc << std::endl;

	using namespace std::filesystem;

	bool showHelp = false;
	std::string config_file;
	std::unique_ptr<cConfigFileData> pConfigData;

	std::string input_directory = current_path().string();
	std::string output_directory = current_path().string();

	auto cli = lyra::cli()
		| lyra::help(showHelp)
		("Show usage information.")
		| lyra::opt(config_file, "configuration")
		["-c"]["--config"]
		("Configuration file to set the options in generating the plot data.")
		.required()
		| lyra::opt(num_of_threads, "threads")
		["-t"]["--threads"]
		("The number of threads to use for repairing data files.")
		.optional()
		| lyra::arg(input_directory, "input directory")
		("The path to input directory for converting point cloud scans to plot scans.")
		.required()
		| lyra::arg(output_directory, "output directory")
		("The path to output directory for the plot file(s).")
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

	pConfigData = std::make_unique<cConfigFileData>(config_file);

	if (!pConfigData->load())
	{
		std::cerr << "Error in configuration file: " << config_file << std::endl;
		std::cerr << std::endl;
		return 2;
	}

	if (output_directory.empty())
		output_directory = input_directory;

	const std::filesystem::path input{ input_directory };
	const std::filesystem::path output{ output_directory };

	std::filesystem::directory_entry output_dir = std::filesystem::directory_entry{ output };

	if (!output_dir.exists())
	{
		std::filesystem::create_directories(output_dir);
	}

	std::vector<directory_entry> lidar_data_files_to_process;

	// Scan input directory for all CERES files to operate on
	for (auto const& dir_entry : std::filesystem::directory_iterator{ input })
	{
		if (!dir_entry.is_regular_file())
			continue;

		// Don't process any ceres files!
		if (dir_entry.path().extension() == ".ceres")
			continue;

		// Test for the lidar_data extension...
		if (dir_entry.path().extension() == ".lidar_data")
		{
			// If there is already a file by the same name in
			// the output directory with a ceres extension,
			// skip the file!
			auto test = output;
			test /= dir_entry.path().filename();
			test.replace_extension("ceres");
			if (std::filesystem::exists(test))
				continue;

			lidar_data_files_to_process.push_back(dir_entry);
		}

	}

	int max_threads = std::thread::hardware_concurrency();

	num_of_threads = std::max(num_of_threads, 0);
	num_of_threads = std::min(num_of_threads, max_threads);

	// Constructs a thread pool with as many threads as available in the hardware.
	BS::thread_pool pool(num_of_threads);
	int n = pool.get_thread_count();

	if (n ==1)
		std::cout << "Using " << n << " thread of a possible " << max_threads << std::endl;
	else
		std::cout << "Using " << n << " threads of a possible " << max_threads << std::endl;

	std::vector<cFileProcessor*> file_processors;

	//=====================================================
	// Convert Lidar_Data files...
	//=====================================================
	for (auto& in_file : lidar_data_files_to_process)
	{
		std::filesystem::path out_file = output;
		out_file /= in_file.path().filename();
		out_file.replace_extension("ceres");

		cFileProcessor* fp = nullptr;	// new cLidarData2CeresConverter();

//		pool.push_task(&cFileProcessor::process_file, fp, in_file, out_file);

		file_processors.push_back(fp);
	}

	lidar_data_files_to_process.clear();

	pool.wait_for_tasks();

	for (auto file_processor : file_processors)
	{
		delete file_processor;
	}

	return 0;
}

