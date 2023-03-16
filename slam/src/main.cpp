
#include "BS_thread_pool.hpp"

#include "FileProcessor.hpp"

#include <lyra/lyra.hpp>

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

	int num_of_threads = 0;
	std::string input_directory = current_path().string();
	std::string output_directory = current_path().string();

	bool isFile = false;
	bool showHelp = false;

	auto cli = lyra::cli()
		| lyra::help(showHelp)
		("Show usage information.")
		| lyra::opt(isFile)
		["-f"]["--file"]
		("Operate on a single file instead of directory.")
		| lyra::opt(num_of_threads, "threads")
		["-t"]["--threads"]
		("The number of threads to use for repairing data files.")
		.optional()
		| lyra::arg(input_directory, "input directory")
		("The path to input directory/file for converting pointcloud data to a ply file(s).")
		.required()
		| lyra::arg(output_directory, "output directory")
		("The path to output directory/file for the ply file(s).")
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

		if (dir_entry.path().extension() == "ceres")
			return 3;

		if (!isCeresFile(dir_entry.path().string()))
			return 4;

		files_to_process.push_back(dir_entry);
	}
	else
	{
		for (auto const& dir_entry : std::filesystem::directory_iterator{ input })
		{
			if (!dir_entry.is_regular_file())
				continue;

			if (dir_entry.path().extension() == "ceres")
				continue;

			if (!isCeresFile(dir_entry.path().string()))
				continue;

			files_to_process.push_back(dir_entry);

			break;
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
		auto fe = removeTimestamp(in_file.path().filename().string());

		if (isFile)
		{
			out_file = std::filesystem::path{ output_directory };
		}

		if (out_file.empty())
		{
			std::string out_filename = fe.filename;
			out_file = output_directory;
			out_file /= add_timestamp(out_filename);

			if (!fe.extension.empty())
			{
				out_file.replace_extension(fe.extension);
			}
		}

		cFileProcessor* fp = new cFileProcessor();

		pool.push_task(&cFileProcessor::process_file, fp, in_file, out_file);

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

