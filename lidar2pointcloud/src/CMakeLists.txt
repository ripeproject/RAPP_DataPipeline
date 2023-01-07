


add_executable(lidar2point)

set_target_properties(lidar2point PROPERTIES LANGUAGE CXX)

target_compile_features(lidar2point PRIVATE cxx_std_20)

target_sources(lidar2point
PRIVATE

	BS_thread_pool.hpp
	ParserExceptions.hpp

	lidar2pointcloud.cpp
)


target_include_directories(lidar2point PRIVATE ${CMAKE_INSTALL_PREFIX}/include)

target_link_libraries(lidar2point PRIVATE cbdf::cbdf)
target_link_libraries(lidar2point PRIVATE cbdf::sensors)

#
# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
set_property(TARGET lidar2point PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")


#******************************************************************************
# Installer
#******************************************************************************


