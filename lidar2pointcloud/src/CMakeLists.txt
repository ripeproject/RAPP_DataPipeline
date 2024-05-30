

set(LIDAR_TO_POINT_CLOUD_COMMON_FILES

	ParserExceptions.hpp

	datatypes.hpp

	LidarMapConfigScan.hpp
	LidarMapConfigScan.cpp
	LidarMapConfigOptions.hpp
	LidarMapConfigOptions.cpp
	LidarMapConfigDefaults.hpp
	LidarMapConfigDefaults.cpp
	LidarMapConfigFile.hpp
	LidarMapConfigFile.cpp

	OusterInfo.hpp
	OusterInfo.cpp
	OusterInfoLoader.hpp
	OusterInfoLoader.cpp

	PointCloudInfo.hpp
	PointCloudInfo.cpp
	PointCloudGenerator.hpp
	PointCloudGenerator.cpp

	KinematicUtils.hpp
	KinematicUtils.cpp

	FieldScanLoader.hpp
	FieldScanLoader.cpp

	PointCloudSaver.hpp
	PointCloudSaver.cpp

	ExportUtils.hpp
	ExportUtils.cpp

	FieldUtils.hpp
	FieldUtils.cpp

	GpsFileReader.hpp
	GpsFileReader.cpp

	GroundModelUtils.hpp
	GroundModelUtils.cpp

	GroundDataModel.hpp
	GroundDataModel.cpp

	RappFieldBoundary.hpp
	RappFieldBoundary.cpp

	RappTriangle.hpp
	RappTriangle.cpp

	RappPointCloud.hpp
	RappPointCloud.cpp

	FieldScanDataModel.hpp
	FieldScanDataModel.cpp

	QuadTree.hpp
	QuadTree.cpp
	QuadNode.hpp
	QuadNode.cpp

	lidar2pointcloud.hpp
	lidar2pointcloud.cpp
	
	FileProcessor.hpp
	FileProcessor.cpp
)


add_executable(console_app)

set_target_properties(console_app PROPERTIES LANGUAGE CXX)
set_target_properties(console_app PROPERTIES OUTPUT_NAME "lidar2pointcloud")

target_compile_features(console_app PRIVATE cxx_std_20)

target_sources(console_app
PRIVATE

	BS_thread_pool.hpp

	${LIDAR_TO_POINT_CLOUD_COMMON_FILES}

	ConsoleApp.cpp
)


target_include_directories(console_app PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
target_include_directories(console_app PRIVATE ${PCL_INCLUDE_DIRS})
target_include_directories(console_app PRIVATE "../support/common")
target_include_directories(console_app PRIVATE "../support/PointCloud")
target_include_directories(console_app PRIVATE "../support/ProcessingInfo")
target_include_directories(console_app PRIVATE "../support/StringUtils")
target_include_directories(console_app PRIVATE "../support/Utilities")
target_include_directories(console_app PRIVATE "../support/MathUtils")

target_link_libraries(console_app PRIVATE ${PCL_LIBRARIES} )
target_link_libraries(console_app PRIVATE Eigen3::Eigen )
target_link_libraries(console_app PRIVATE dlib::dlib )
target_link_libraries(console_app PRIVATE tinyply)
target_link_libraries(console_app PRIVATE cbdf::cbdf)
target_link_libraries(console_app PRIVATE cbdf::info)
target_link_libraries(console_app PRIVATE cbdf::ctrl)
target_link_libraries(console_app PRIVATE cbdf::gps cbdf::lidar)
target_link_libraries(console_app PRIVATE pointcloud_io)
target_link_libraries(console_app PRIVATE processing_info)
target_link_libraries(console_app PRIVATE string_utils)
target_link_libraries(console_app PRIVATE math_utils)
target_link_libraries(console_app PRIVATE utilities)
target_link_libraries(console_app PRIVATE fmt::fmt)


#
# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
set_property(TARGET console_app PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")


if(wxWidgets_FOUND)

	if(WIN32)
		# Include a RC file for windows
		set(resource_files
			./Resources/ripe.ico
			./Resources/LidarConvert.rc
		)

		source_group("Resources" FILES
			${resource_files}
		)

	elseif(APPLE)

		# Add an icon for the apple .app file
		set(resource_files
			Resources/ripe.icns
		)

	endif()

	add_executable(gui_app WIN32 MACOSX_BUNDLE)

	set_target_properties(gui_app PROPERTIES LANGUAGE CXX)
	set_target_properties(gui_app PROPERTIES OUTPUT_NAME "LidarConvert")

	target_compile_features(gui_app PRIVATE cxx_std_20)

	target_sources(gui_app
	PRIVATE

		MainWindow.hpp
		MainWindow.cpp
		MainFrame.hpp
		MainFrame.cpp

		${LIDAR_TO_POINT_CLOUD_COMMON_FILES}
		${resource_files}
		
		GuiApp.cpp
	)

	include(${wxWidgets_USE_FILE})

	target_include_directories(gui_app PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
	target_include_directories(gui_app PRIVATE ${PCL_INCLUDE_DIRS})
	target_include_directories(gui_app PRIVATE "../support/common")
	target_include_directories(gui_app PRIVATE "../support/PointCloud")
	target_include_directories(gui_app PRIVATE "../support/ProcessingInfo")
	target_include_directories(gui_app PRIVATE "../support/StringUtils")
	target_include_directories(gui_app PRIVATE "../support/wxCustomWidgets")
	target_include_directories(gui_app PRIVATE "../support/MathUtils")

	target_link_libraries(gui_app PRIVATE ${CMAKE_THREAD_LIBS_INIT})
	target_link_libraries(gui_app PRIVATE ${wxWidgets_LIBRARIES})
	target_link_libraries(gui_app PRIVATE ${PCL_LIBRARIES} )
	target_link_libraries(gui_app PRIVATE Eigen3::Eigen )
	target_link_libraries(gui_app PRIVATE dlib::dlib )
	target_link_libraries(gui_app PRIVATE tinyply)
	target_link_libraries(gui_app PRIVATE cbdf::cbdf)
	target_link_libraries(gui_app PRIVATE cbdf::info)
	target_link_libraries(gui_app PRIVATE cbdf::ctrl)
	target_link_libraries(gui_app PRIVATE cbdf::gps cbdf::lidar)
	target_link_libraries(gui_app PRIVATE pointcloud_io)
	target_link_libraries(gui_app PRIVATE processing_info)
	target_link_libraries(gui_app PRIVATE string_utils)
	target_link_libraries(gui_app PRIVATE math_utils)
	target_link_libraries(gui_app PRIVATE wxCustomWidgets)

	#
	# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
	# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
	set_property(TARGET gui_app PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

endif()



#******************************************************************************
# Installer
#******************************************************************************


