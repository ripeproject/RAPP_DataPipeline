
#include "CeresFileVerifier.hpp"
#include "LidarFileVerifier.hpp"
#include "BS_thread_pool.hpp"


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
}

void console_message(const std::string& msg)
{
}

void new_file_progress(const int id, std::string filename)
{
}

void update_prefix_progress(const int id, std::string prefix, const int progress_pct)
{
}

void update_progress(const int id, const int progress_pct)
{
}

void complete_file_progress(const int id, std::string prefix, std::string suffix)
{
}


int main(int argc, char** argv)
{
	using namespace std::filesystem;

	return 0;
}

