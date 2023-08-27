
#include "FileProcessor.hpp"
#include "BS_thread_pool.hpp"

#include <lyra/lyra.hpp>

#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>


std::mutex g_console_mutex;

namespace
{
	int numFilesToProcess = 0;
}

void console_message(const std::string& msg)
{
	std::lock_guard<std::mutex> guard(g_console_mutex);
	std::cout << msg << std::endl;
}

void new_file_progress(const int id, std::string filename)
{
}

void update_file_progress(const int id, std::string filename, const int progress_pct)
{
}

void update_file_progress(const int id, const int progress_pct)
{
}


int main(int argc, char** argv)
{
	using namespace std::filesystem;

	// The default is one thread because this application in I/O bound.
	int num_of_threads = 1;
	bool showHelp = false;

	std::string input_directory = current_path().string();

	auto cli = lyra::cli()
		| lyra::help(showHelp)
		("Show usage information.")
		| lyra::opt(num_of_threads, "threads")
		["-t"]["--threads"]
		("The number of threads to use for repairing data files.")
		.optional()
		| lyra::arg(input_directory, "input directory")
		("The path to input directory for repairing of ceres data.");

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
	const std::filesystem::path failed_dir = input / "failed";

	if (!std::filesystem::exists(failed_dir))
	{
		// If the failed directory does not exists, we are done!
		return 0;
	}

	std::vector<directory_entry> files_to_repair;
	for (auto const& dir_entry : std::filesystem::directory_iterator{ failed_dir })
	{
		if (!dir_entry.is_regular_file())
			continue;

		if (dir_entry.path().extension() != ".ceres")
			continue;

		files_to_repair.push_back(dir_entry);
	}

	if (files_to_repair.empty())
	{
		// No files to repair!
		return 0;
	}

	const std::filesystem::path recovered_dir = failed_dir / "recovered";
	if (!std::filesystem::exists(recovered_dir))
	{
		std::filesystem::create_directory(recovered_dir);
	}

	const std::filesystem::path repaired_dir = input / "repaired";
	if (!std::filesystem::exists(repaired_dir))
	{
		std::filesystem::create_directory(repaired_dir);
	}

	int max_threads = std::thread::hardware_concurrency();

	num_of_threads = std::max(num_of_threads, 0);
	num_of_threads = std::min(num_of_threads, max_threads);

	// Constructs a thread pool with the desired number of threads.
	BS::thread_pool pool(num_of_threads);
	int n = pool.get_thread_count();

	if (n == 1)
		std::cout << "Using " << n << " thread of a possible " << max_threads << std::endl;
	else
		std::cout << "Using " << n << " threads of a possible " << max_threads << std::endl;

	std::vector<cFileProcessor*> file_processors;

	int numFilesToProcess = 0;
	for (auto& file : files_to_repair)
	{
		cFileProcessor* fp = new cFileProcessor(numFilesToProcess++, recovered_dir, repaired_dir);
		
		if (fp->setFileToRepair(file))
		{
			pool.push_task(&cFileProcessor::process_file, fp);
		}

		file_processors.push_back(fp);
	}

	pool.wait_for_tasks();

	for (auto file_processor : file_processors)
	{
		delete file_processor;
	}

	return 0;
}

