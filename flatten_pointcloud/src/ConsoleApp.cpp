
#include "BS_thread_pool.hpp"

#include "DataModelUtils.hpp"
#include "FileProcessor.hpp"
#include "StringUtils.hpp"

#include "TextProgressBar.hpp"

#include <lyra/lyra.hpp>

#include <eigen3/Eigen/Eigen>

#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <numbers>


std::mutex g_console_mutex;

namespace
{
	int numFilesToProcess = 0;
	cTextProgressBar progress_bar;
}

void console_message(const std::string& msg)
{
	std::lock_guard<std::mutex> guard(g_console_mutex);
	std::cout << "\n" << msg << std::endl;
}

void new_file_progress(const int id, std::string filename)
{
	progress_bar.addProgressEntry(id, filename);
}

void update_prefix_progress(const int id, std::string prefix, const int progress_pct)
{
	progress_bar.updateProgressEntry(id, prefix, progress_pct);
}

void update_progress(const int id, const int progress_pct)
{
	progress_bar.updateProgressEntry(id, progress_pct);
}

void complete_file_progress(const int id)
{
	progress_bar.finishProgressEntry(id);
}


int main(int argc, char** argv)
{
	using namespace std::filesystem;
	using namespace nStringUtils;

	int num_of_threads = 1;

	bool isFile = false;
	bool showHelp = false;
	std::string ground_data = current_path().string() + "FullField.csv";

	std::string input_directory = current_path().string();
	std::string output_directory = current_path().string();

	auto cli = lyra::cli()
		| lyra::help(showHelp)
		("Show usage information.")
		| lyra::opt(ground_data, "Ground data source")
		["-g"]["--ground_data"]
		("The file containing the GPS ground point data.")
		.required()
		| lyra::opt(isFile)
		["-f"]["--file"]
		("Operate on a single file instead of directory.")
		| lyra::opt(num_of_threads, "threads")
		["-t"]["--threads"]
		("The number of threads to use for flattening data files.")
		.optional()
		| lyra::arg(input_directory, "input directory")
		("The path to input directory/file for flattening pointcloud data.")
		.required()
		| lyra::arg(output_directory, "output directory")
		("The path to output directory/file of the flattening pointcloud data.")
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

	if (!std::filesystem::exists(input_directory))
	{
		if (isFile)
			std::cerr << "Input file " << ground_data << " does not exists." << std::endl;
		else
			std::cerr << "Input directory " << input_directory << " does not exists." << std::endl;
		return 1;
	}

	// Bail out if we can't load the ground data
	if (!std::filesystem::exists(ground_data))
	{
		std::cerr << "Error: " << ground_data << " does not exists." << std::endl;
		return 1;
	}

	if (!load_ground_data(ground_data))
	{
		std::cerr << "Error: " << ground_data << " could not be loaded." << std::endl;
		return 1;
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
	std::vector<cFileProcessor*> file_processors;

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
				out_file.replace_extension(fe.extension);
			}
		}

		cFileProcessor* fp = new cFileProcessor(numFilesToProcess++, in_file, out_file);

		pool.push_task(&cFileProcessor::process_file, fp);

		file_processors.push_back(fp);
	}

	progress_bar.setMaxID(numFilesToProcess);

	files_to_process.clear();

	if (output_dir.exists() && (numFilesToProcess == 0))
	{
		if (std::filesystem::remove(output_dir))
		{
			std::cout << "The empty directory " << output_dir << " was removed." << std::endl;
		}

		return 0;
	}

	pool.wait_for_tasks();

	for (auto file_processor : file_processors)
	{
		delete file_processor;
	}

	return 0;
}

