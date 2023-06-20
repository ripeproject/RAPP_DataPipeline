

#include <lyra/lyra.hpp>

#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <numbers>


int main(int argc, char** argv)
{
	using namespace std::filesystem;

	std::string input_directory;
	std::string cfg_filename = "crop_ops.json";
	bool showHelp = false;

	auto cli = lyra::cli()
		| lyra::help(showHelp)
		("Show usage information.")
		| lyra::opt(cfg_filename, "config filename")
		["-c"]["--config"]
		("Specify the name of the configuration file to use.")
		.optional()
		| lyra::arg(input_directory, "input directory")
		("The path to input directory for analyzing lidar data.");

	auto result = cli.parse({argc, argv});

	if (!result)
	{
		std::cerr << "Error in command line: " << result.message() << std::endl;
		return 1;
	}

	const std::filesystem::path input{ input_directory };
	
	return 0;
}

