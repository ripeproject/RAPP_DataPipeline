
#include "CeresFileVerifier.hpp"
#include "LidarFileVerifier.hpp"
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

std::atomic<uint32_t> g_num_failed_files = 0;

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

void complete_file_progress(const int id, std::string filename, std::string suffix)
{
	progress_bar.finishProgressEntry(id, filename, suffix);
}


int main(int argc, char** argv)
{
	using namespace std::filesystem;

	// The default is to use only one thread as this application is I/O limited.
	int num_of_threads = 1;
	bool showHelp = false;

	std::string input_directory = current_path().string();

	auto cli = lyra::cli()
		| lyra::help(showHelp)
		("Show usage information.")
		| lyra::opt(num_of_threads, "threads")
		["-t"]["--threads"]
		("The number of threads to use for verification.")
		.optional()
		| lyra::arg(input_directory, "input directory")
		("The path to input directory for verification of ceres data.");

	auto result = cli.parse({argc, argv});

	if (showHelp)
	{
		std::cout << cli << std::endl;
		return 0;
	}

	if (!result)
	{
		std::cerr << "Error in command line: " << result.message() << std::endl;
		return -1;
	}

	const std::filesystem::path input{ input_directory };
	const std::filesystem::path failed = input / "failed_files";

	std::vector<directory_entry> ceres_files_to_check;
	for (auto const& dir_entry : std::filesystem::directory_iterator{ input })
	{
		if (!dir_entry.is_regular_file())
			continue;

		if (dir_entry.path().extension() != ".ceres")
			continue;

		ceres_files_to_check.push_back(dir_entry);
	}

	std::vector<directory_entry> lidar_files_to_check;
	for (auto const& dir_entry : std::filesystem::directory_iterator{ input })
	{
		if (!dir_entry.is_regular_file())
			continue;

		if (dir_entry.path().extension() != ".lidar_data")
			continue;

		lidar_files_to_check.push_back(dir_entry);
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

	std::vector<cCeresFileVerifier*> ceres_file_verifiers;

	for (auto& file : ceres_files_to_check)
	{
		cCeresFileVerifier* fv = new cCeresFileVerifier(numFilesToProcess++, failed);
		fv->setFileToCheck(file);

		pool.push_task(&cCeresFileVerifier::process_file, fv);

		ceres_file_verifiers.push_back(fv);
	}

	std::vector<cLidarFileVerifier*> lidar_file_verifiers;

	for (auto& file : lidar_files_to_check)
	{
		cLidarFileVerifier* fv = new cLidarFileVerifier(numFilesToProcess++, failed);
		fv->setFileToCheck(file);

		pool.push_task(&cLidarFileVerifier::process_file, fv);

		lidar_file_verifiers.push_back(fv);
	}

	progress_bar.setMaxID(numFilesToProcess);

	pool.wait_for_tasks();

	for (auto file_verifier : ceres_file_verifiers)
	{
		delete file_verifier;
	}

	for (auto file_verifier : lidar_file_verifiers)
	{
		delete file_verifier;
	}

	return g_num_failed_files;
}

