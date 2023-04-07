


add_executable(lidar2pointcloud)

set_target_properties(lidar2pointcloud PROPERTIES LANGUAGE CXX)

target_compile_features(lidar2pointcloud PRIVATE cxx_std_20)

target_sources(lidar2pointcloud
PRIVATE

	BS_thread_pool.hpp
	ParserExceptions.hpp

	Kinematics.hpp
	Kinematics_Constant.hpp
	Kinematics_Constant.cpp
	Kinematics_Dolly.hpp
	Kinematics_Dolly.cpp
	Kinematics_GPS.hpp
	Kinematics_GPS.cpp
	Kinematics_SLAM.hpp
	Kinematics_SLAM.cpp

	lidar2pointcloud.hpp
	lidar2pointcloud.cpp
	
	FileProcessor.hpp
	FileProcessor.cpp
	main.cpp
)


target_include_directories(lidar2pointcloud PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
target_include_directories(lidar2pointcloud PRIVATE "../support/PointCloud")
target_include_directories(lidar2pointcloud PRIVATE "../support/StringUtils")

target_link_libraries(lidar2pointcloud PRIVATE cbdf::cbdf)
target_link_libraries(lidar2pointcloud PRIVATE cbdf::sensors)
target_link_libraries(lidar2pointcloud PRIVATE pointcloud_io)
target_link_libraries(lidar2pointcloud PRIVATE string_utils)


#
# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
set_property(TARGET lidar2pointcloud PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")


#******************************************************************************
# Installer
#******************************************************************************


