
#include "DataVerifier.hpp"
#include "BS_thread_pool.hpp"

#include <lyra/lyra.hpp>

#include <filesystem>
#include <string>
#include <vector>
#include <iostream>

int main(int argc, char** argv)
{
	using namespace std::filesystem;

	int num_of_threads = 0;
	std::string input_directory = current_path().string();

	auto cli = lyra::cli()
		| lyra::opt(num_of_threads, "threads")
		["-t"]["--threads"]
		("The number of threads to use for verification.")
		.optional()
		| lyra::arg(input_directory, "input directory")
		("The path to input directory for verification of ceres data.");

	auto result = cli.parse({argc, argv});

	if (!result)
	{
		std::cerr << "Error in command line: " << result.message() << std::endl;
		return 1;
	}

	const std::filesystem::path input{ input_directory };
	const std::filesystem::path failed = input / "failed";

	std::vector<directory_entry> files_to_check;
	for (auto const& dir_entry : std::filesystem::directory_iterator{ input })
	{
		if (!dir_entry.is_regular_file())
			continue;

		if (dir_entry.path().extension() == "ceres")
			continue;

		files_to_check.push_back(dir_entry);
	}

	int max_threads = std::thread::hardware_concurrency();

	num_of_threads = std::max(num_of_threads, 0);
	num_of_threads = std::min(num_of_threads, max_threads);

	// Constructs a thread pool with as many threads as available in the hardware.
	BS::thread_pool pool(num_of_threads);
	int n = pool.get_thread_count();

	std::cout << "Using " << n << " threads of a possible " << max_threads << std::endl;

	std::vector<cDataVerifier*> data_verifiers;

	for (auto& file : files_to_check)
	{
		cDataVerifier* dv = new cDataVerifier(failed);

		pool.push_task(&cDataVerifier::process_file, dv, file);

		data_verifiers.push_back(dv);
	}

	pool.wait_for_tasks();

	for (auto data_verifier : data_verifiers)
	{
		delete data_verifier;
	}

	return 0;
}

