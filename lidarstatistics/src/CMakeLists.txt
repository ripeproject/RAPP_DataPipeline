


add_executable(lidar_stats)

set_target_properties(lidar_stats PROPERTIES LANGUAGE CXX)

target_compile_features(lidar_stats PRIVATE cxx_std_20)

target_sources(lidar_stats
PRIVATE

	BS_thread_pool.hpp
	ParserExceptions.hpp

	LidarStatistics.hpp
	LidarStatistics.cpp
	
	FileProcessor.hpp
	FileProcessor.cpp
	main.cpp
)


target_include_directories(lidar_stats PRIVATE ${CMAKE_INSTALL_PREFIX}/include)

target_link_libraries(lidar_stats PRIVATE cbdf::cbdf)
target_link_libraries(lidar_stats PRIVATE cbdf::lidar)
target_link_libraries(lidar_stats PRIVATE fmt::fmt)

#
# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
set_property(TARGET lidar_stats PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")


#******************************************************************************
# Installer
#******************************************************************************


