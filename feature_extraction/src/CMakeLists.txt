

add_executable(feature_extraction)

set_target_properties(feature_extraction PROPERTIES LANGUAGE CXX)

target_compile_features(feature_extraction PRIVATE cxx_std_20)

target_sources(feature_extraction
PRIVATE

	nanoflann.hpp
	utils.h
	
	BS_thread_pool.hpp
	ParserExceptions.hpp

	CommonFunctions.h
	CommonFunctions.cpp

	LineDetection3D.h
	LineDetection3D.cpp

	ExtractFeatures.hpp
	ExtractFeatures.cpp
	
	FileProcessor.hpp
	FileProcessor.cpp
	main.cpp
)

target_include_directories(feature_extraction PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
target_include_directories(feature_extraction PRIVATE "../support/PointCloud")

target_link_libraries(feature_extraction PRIVATE cbdf::cbdf)
target_link_libraries(feature_extraction PRIVATE pointcloud_io)
target_link_libraries(feature_extraction PRIVATE tinyply)
target_link_libraries(feature_extraction PRIVATE ${OpenCV_LIBS})

#
# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
set_property(TARGET feature_extraction PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")


#******************************************************************************
# Installer
#******************************************************************************


