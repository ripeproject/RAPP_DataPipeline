

add_subdirectory(bdf_v1)


add_executable(convert2ceres)

set_target_properties(convert2ceres PROPERTIES LANGUAGE CXX)

target_compile_features(convert2ceres PRIVATE cxx_std_20)

target_sources(convert2ceres
PRIVATE

	BS_thread_pool.hpp
	ParserExceptions.hpp

	LidarData2CeresConverter.hpp
	LidarData2CeresConverter.cpp

	FileProcessor.hpp
	FileProcessor.cpp
	main.cpp
)

target_include_directories(convert2ceres PRIVATE ${CMAKE_INSTALL_PREFIX}/include)

target_link_libraries(convert2ceres PRIVATE BlockDataFile_v1)
target_link_libraries(convert2ceres PRIVATE cbdf::cbdf)
target_link_libraries(convert2ceres PRIVATE cbdf::info)
target_link_libraries(convert2ceres PRIVATE cbdf::sensors)

#
# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
set_property(TARGET convert2ceres PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")


#******************************************************************************
# Installer
#******************************************************************************


