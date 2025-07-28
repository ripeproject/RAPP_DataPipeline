
#include "FileRepairProcessor.hpp"
#include "BS_thread_pool.hpp"

#include "TextProgressBar.hpp"

#include <lyra/lyra.hpp>

#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <atomic>


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

void complete_file_progress(const int id, std::string prefix, std::string suffix)
{
	progress_bar.finishProgressEntry(id, prefix, suffix);
}


int main(int argc, char** argv)
{
	using namespace std::filesystem;
	using namespace ceres_file_repair;

	// The default is one thread because this application in I/O bound.
	int num_of_threads = 1;
	bool showHelp = false;
	bool verboseMode = false;
	bool quietMode = false;

	std::string input_directory = current_path().string();

	auto cli = lyra::cli()
		| lyra::help(showHelp)
		("Show usage information.")
		| lyra::opt(num_of_threads, "threads")
		["-t"]["--threads"]
		("The number of threads to use for repairing data files.")
		.optional()
		| lyra::opt(verboseMode, "verbose mode")
		["-v"]["--verbose"]
		("Enable verbose mode.")
		.optional()
		| lyra::opt(quietMode, "quiet mode")
		["-q"]["--quiet"]
		("Enable quiet mode: no status output to screen.")
		.optional()
		| lyra::arg(input_directory, "input directory")
		("The path to input directory for repairing of ceres data files.");

	auto result = cli.parse({argc, argv});

	if (showHelp)
	{
		std::cout << cli << std::endl;
		return 0;
	}

	if (!result)
	{
		std::cerr << "Error in command line: " << result.message() << std::endl;
		return 1;
	}

	const std::filesystem::path input{ input_directory };

	std::vector<directory_entry> files_to_repair;
	for (auto const& dir_entry : std::filesystem::directory_iterator{ input })
	{
		if (!dir_entry.is_regular_file())
			continue;

		if (dir_entry.path().extension() != ".ceres")
			continue;

		files_to_repair.push_back(dir_entry);
	}

	if (files_to_repair.empty())
	{
		std::cerr << "No files to repair!" << std::endl;
		return 0;
	}

	const std::filesystem::path temporary_dir = input / "tmp";
	if (!std::filesystem::exists(temporary_dir))
	{
		std::filesystem::create_directory(temporary_dir);
	}

	const std::filesystem::path partial_repaired_dir = input / "partial_repaired_files";
	const std::filesystem::path fully_repaired_dir = input / "fully_repaired_files";

	int max_threads = std::thread::hardware_concurrency();

	num_of_threads = std::max(num_of_threads, 0);
	num_of_threads = std::min(num_of_threads, max_threads);

	// Constructs a thread pool with the desired number of threads.
	BS::thread_pool pool(num_of_threads);
	int n = pool.get_thread_count();

	if (verboseMode)
	{
		if (n == 1)
			std::cout << "Using " << n << " thread of a possible " << max_threads << std::endl;
		else
			std::cout << "Using " << n << " threads of a possible " << max_threads << std::endl;
	}

	std::vector<cFileRepairProcessor*> file_processors;

	int numFilesToProcess = 0;
	for (auto& file : files_to_repair)
	{
		cFileRepairProcessor* fp = new cFileRepairProcessor(numFilesToProcess++,
								temporary_dir, partial_repaired_dir, fully_repaired_dir);
		
		if (fp->setFileToRepair(file))
		{
			pool.push_task(&cFileRepairProcessor::process_file, fp);
		}

		file_processors.push_back(fp);
	}

	progress_bar.setQuietMode(quietMode);
	progress_bar.setVerboseMode(verboseMode);
	progress_bar.setMaxID(numFilesToProcess);

	pool.wait_for_tasks();

	for (auto file_processor : file_processors)
	{
		delete file_processor;
	}

	if (std::filesystem::is_empty(temporary_dir))
	{
		std::filesystem::remove(temporary_dir);
	}

	if (!quietMode)
	{
		if (g_num_partial_files == 0)
		{
			std::cout << "All " << numFilesToProcess << " files repaired!" << std::endl;
		}
		else
		{
			std::cout << (numFilesToProcess - g_num_partial_files) << " files repaired , " << g_num_partial_files << " could not be repaired!" << std::endl;
		}
	}

	return g_num_partial_files;
}

