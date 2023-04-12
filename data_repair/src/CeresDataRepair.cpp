
#include "DataRepair.hpp"
#include "BS_thread_pool.hpp"

#include <lyra/lyra.hpp>

#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>


std::mutex g_console_mutex;


void console_message(const std::string& msg)
{
	std::lock_guard<std::mutex> guard(g_console_mutex);
	std::cout << msg << std::endl;
}


int main(int argc, char** argv)
{
	using namespace std::filesystem;

	// The default is one thread because this application in I/O bound.
	int num_of_threads = 1;
	std::string input_directory = current_path().string();

	auto cli = lyra::cli()
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

	const std::filesystem::path input{ input_directory };
	const std::filesystem::path failed = input / "failed";

	if (!std::filesystem::exists(failed))
	{
		// If the failed directory does not exists, we are done!
		return 0;
	}

	std::vector<directory_entry> files_to_repair;
	for (auto const& dir_entry : std::filesystem::directory_iterator{ failed })
	{
		if (!dir_entry.is_regular_file())
			continue;

		if (dir_entry.path().extension() == "ceres")
			continue;

		files_to_repair.push_back(dir_entry);
	}

	if (files_to_repair.empty())
	{
		// No files to repair!
		return 0;
	}

	const std::filesystem::path repaired = input / "repaired";
	if (!std::filesystem::exists(repaired))
	{
		std::filesystem::create_directory(repaired);
	}

	int max_threads = std::thread::hardware_concurrency();

	num_of_threads = std::max(num_of_threads, 0);
	num_of_threads = std::min(num_of_threads, max_threads);

	// Constructs a thread pool with the desired number of threads.
	BS::thread_pool pool(num_of_threads);
	int n = pool.get_thread_count();

	std::cout << "Using " << n << " threads of a possible " << max_threads << std::endl;

	std::vector<cDataRepair*> data_repairs;

	for (auto& file : files_to_repair)
	{
		cDataRepair* dv = new cDataRepair(repaired);

		pool.push_task(&cDataRepair::process_file, dv, file);

		data_repairs.push_back(dv);
	}

	pool.wait_for_tasks();

	for (auto data_repair : data_repairs)
	{
		delete data_repair;
	}

	return 0;
}

