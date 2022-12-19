
#include "DataVerifier.hpp"

#include <lyra/lyra.hpp>

#include <filesystem>
#include <string>

int main(int argc, char** argv)
{
	using namespace std::filesystem;

	std::string input_directory = current_path().string();

	auto cli = lyra::cli()
		| lyra::arg(input_directory, "input directory")
		("The path to input directory for verification of ceres data.");

	const std::filesystem::path input{ input_directory };

	for (auto const& dir_entry : std::filesystem::directory_iterator{ input })
	{
		std::cout << dir_entry.path() << '\n';
	}

	return 0;
}

