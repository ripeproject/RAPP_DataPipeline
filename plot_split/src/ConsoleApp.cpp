
#include "BS_thread_pool.hpp"

#include "FileProcessor.hpp"

#include "PlotConfigFile.hpp"

#include "TextProgressBar.hpp"

#include "StringUtils.hpp"

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
	using namespace nStringUtils;


	bool isFile = false;
	bool showHelp = false;
	std::string config_file;

	std::string input_directory = current_path().string();
	std::string output_directory = current_path().string();

	auto cli = lyra::cli()
		| lyra::help(showHelp)
		("Show usage information.")
		| lyra::opt(config_file, "configuration")
		["-c"]["--config"]
		("Configuration file to set the options in generating the plot data.")
		.required()
		| lyra::opt(isFile)
		["-f"]["--file"]
		("Operate on a single file instead of directory.")
		.optional()
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

	// Bail out if the input data does not exist
	if (!std::filesystem::exists(input_directory))
	{
		if (isFile)
			std::cerr << "Input file " << input_directory << " does not exists." << std::endl;
		else
			std::cerr << "Input directory " << input_directory << " does not exists." << std::endl;
		return 1;
	}

	// Bail out if we can't load the configuration data
	cPlotConfigFile configData;
	if (!configData.open(config_file))
	{
		std::cerr << "Error in configuration file: " << config_file << std::endl;
		std::cerr << std::endl;
		return 2;
	}

	if (output_directory.empty())
		output_directory = input_directory;

	const std::filesystem::path input{ input_directory };

	std::vector<directory_entry> files_to_process;

	std::string month_dir;

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
		if (input.has_parent_path())
		{
			month_dir = input.filename().string();
			if (!nStringUtils::isMonthDirectory(month_dir))
				month_dir.clear();
		}

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

/*
	if (n == 1)
		std::cout << "Using " << n << " thread of a possible " << max_threads << std::endl;
	else
		std::cout << "Using " << n << " threads of a possible " << max_threads << std::endl;
*/

	/*
	 * Setup the output directory
	 */

	std::filesystem::path output{ output_directory };
	std::filesystem::directory_entry output_dir;

	if (isFile && output.has_extension())
	{
		output_dir = std::filesystem::directory_entry{ output.parent_path() };
	}
	else
	{
		if (!month_dir.empty())
		{
			std::string last_dir;
			if (output.has_parent_path())
			{
				last_dir = output.filename().string();
			}

			if (month_dir != last_dir)
			{
				output /= month_dir;
			}
		}

		output_dir = std::filesystem::directory_entry{ output };
	}

	std::vector<cFileProcessor*> file_processors;

	/*
	 * Add all of the files to process to the thread pool
	 */
	const auto& options = configData.getOptions();

	for (auto& in_file : files_to_process)
	{
		auto fe = removeProcessedTimestamp(in_file.path().filename().string());

		std::filesystem::path out_file = output_dir;

		out_file /= fe.filename;

		auto it = configData.find_by_filename(in_file.path().filename().string());
		if (it == configData.end())
			continue;

		cFileProcessor* fp = new cFileProcessor(numFilesToProcess, in_file, out_file, *it);

		numFilesToProcess += 1;

		fp->savePlotsInSingleFile(options.getSavePlotsInSingleFile());
		fp->savePlyFiles(options.getSavePlyFiles());
		fp->plyUseBinaryFormat(options.getPlysUseBinaryFormat());
		fp->enableSavingFrameIDs(options.getSaveFrameIds());
		fp->enableSavingPixelInfo(options.getSavePixelInfo());

		pool.push_task(&cFileProcessor::process_file, fp);

		file_processors.push_back(fp);
	}

	if (!output_dir.exists() && (numFilesToProcess > 0))
	{
		std::filesystem::create_directories(output_dir);
	}

	progress_bar.setMaxID(numFilesToProcess);

	files_to_process.clear();

	pool.wait_for_tasks();

	for (auto file_processor : file_processors)
	{
		delete file_processor;
	}

	return 0;
}

