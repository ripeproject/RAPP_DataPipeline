


add_executable(pointcloud2ply)

set_target_properties(pointcloud2ply PROPERTIES LANGUAGE CXX)

target_compile_features(pointcloud2ply PRIVATE cxx_std_20)

target_sources(pointcloud2ply
PRIVATE

	BS_thread_pool.hpp
	ParserExceptions.hpp

	pointcloud2ply.hpp
	pointcloud2ply.cpp
	
	FileProcessor.hpp
	FileProcessor.cpp
	main.cpp
)


target_include_directories(pointcloud2ply PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
target_include_directories(pointcloud2ply PRIVATE "../support/PointCloud")
target_include_directories(pointcloud2ply PRIVATE "../support/StringUtils")

target_link_libraries(pointcloud2ply PRIVATE cbdf::cbdf)
target_link_libraries(pointcloud2ply PRIVATE cbdf::ctrl)
target_link_libraries(pointcloud2ply PRIVATE pointcloud_io)
target_link_libraries(pointcloud2ply PRIVATE string_utils)
target_link_libraries(pointcloud2ply PRIVATE tinyply)


#
# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
set_property(TARGET pointcloud2ply PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")


#******************************************************************************
# Installer
#******************************************************************************


