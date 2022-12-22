
#include "DataVerifier.hpp"

#include <lyra/lyra.hpp>

#include <filesystem>
#include <string>
#include <vector>

int main(int argc, char** argv)
{
	using namespace std::filesystem;

	std::string input_directory = current_path().string();

	auto cli = lyra::cli()
		| lyra::arg(input_directory, "input directory")
		("The path to input directory for verification of ceres data.");

	auto result = cli.parse({argc, argv});

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

	cDataVerifier dv(failed);

	for (auto& file : files_to_check)
	{
		if (!dv.open(file) )
			continue;

		dv.run();
	}

	return 0;
}

