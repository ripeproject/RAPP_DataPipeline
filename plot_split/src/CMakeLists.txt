


set(PLOT_SPLIT_COMMON_FILES

	ParserExceptions.hpp

	FileProcessor.hpp
	FileProcessor.cpp

#	PlotConfigFile.hpp
#	PlotConfigFile.cpp

#	PlotConfigOptions.hpp
#	PlotConfigOptions.cpp

#	PlotConfigScan.hpp
#	PlotConfigScan.cpp

#	PlotConfigBoundary.hpp
#	PlotConfigBoundary.cpp

#	PlotConfigIsolationMethod.hpp
#	PlotConfigIsolationMethod.cpp

	PointCloudInfo.hpp
	PointCloudInfo.cpp
	PointCloudLoader.hpp
	PointCloudLoader.cpp
	
	ExportUtils.hpp
	ExportUtils.cpp
)


add_executable(console_app)

set_target_properties(console_app PROPERTIES LANGUAGE CXX)
set_target_properties(console_app PROPERTIES OUTPUT_NAME "plot_splitter")

target_compile_features(console_app PRIVATE cxx_std_20)

target_sources(console_app
PRIVATE

	BS_thread_pool.hpp
	
	${PLOT_SPLIT_COMMON_FILES}
	
	ConsoleApp.cpp
)

target_include_directories(console_app PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
target_include_directories(console_app PRIVATE "../support/PlotUtils")
target_include_directories(console_app PRIVATE "../support/PlotConfig")
target_include_directories(console_app PRIVATE "../support/PointCloudUtils")
target_include_directories(console_app PRIVATE "../support/StringUtils")
target_include_directories(console_app PRIVATE "../support/Utilities")
target_include_directories(console_app PRIVATE "../support/MathUtils")
target_include_directories(console_app PRIVATE "../support/common")

target_link_libraries(console_app PRIVATE cbdf::cbdf)
target_link_libraries(console_app PRIVATE cbdf::info)
target_link_libraries(console_app PRIVATE cbdf::processing_info)
target_link_libraries(console_app PRIVATE cbdf::pointcloud)
target_link_libraries(console_app PRIVATE cbdf::plot_info)
target_link_libraries(console_app PRIVATE nlohmann_json)
target_link_libraries(console_app PRIVATE pointcloud_utils)
target_link_libraries(console_app PRIVATE plot_utils)
target_link_libraries(console_app PRIVATE plot_config)
target_link_libraries(console_app PRIVATE string_utils)
target_link_libraries(console_app PRIVATE utilities)
target_link_libraries(console_app PRIVATE math_utils)
target_link_libraries(console_app PRIVATE fmt::fmt)
target_link_libraries(console_app PRIVATE tinyply)

#
# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
set_property(TARGET console_app PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")


if(wxWidgets_FOUND)

	if(WIN32)
		# Include a RC file for windows
		set(resource_files
			./Resources/ripe.ico
			./Resources/PlotSplit.rc
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
	set_target_properties(gui_app PROPERTIES OUTPUT_NAME "PlotSplit")

	target_compile_features(gui_app PRIVATE cxx_std_20)

	target_sources(gui_app
	PRIVATE

		MainWindow.hpp
		MainWindow.cpp
		MainFrame.hpp
		MainFrame.cpp

		${PLOT_SPLIT_COMMON_FILES}
		${resource_files}
		
		GuiApp.cpp
	)

	include(${wxWidgets_USE_FILE})

	target_include_directories(gui_app PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
	target_include_directories(gui_app PRIVATE "../support/PlotUtils")
	target_include_directories(gui_app PRIVATE "../support/PlotConfig")
	target_include_directories(gui_app PRIVATE "../support/PointCloudUtils")
	target_include_directories(gui_app PRIVATE "../support/StringUtils")
	target_include_directories(gui_app PRIVATE "../support/wxCustomWidgets")
	target_include_directories(gui_app PRIVATE "../support/MathUtils")
	target_include_directories(gui_app PRIVATE "../support/common")

	target_link_libraries(gui_app PRIVATE ${CMAKE_THREAD_LIBS_INIT})
	target_link_libraries(gui_app PRIVATE ${wxWidgets_LIBRARIES})
	target_link_libraries(gui_app PRIVATE cbdf::cbdf)
	target_link_libraries(gui_app PRIVATE cbdf::info)
	target_link_libraries(gui_app PRIVATE cbdf::processing_info)
	target_link_libraries(gui_app PRIVATE cbdf::pointcloud)
	target_link_libraries(gui_app PRIVATE cbdf::plot_info)
	target_link_libraries(gui_app PRIVATE nlohmann_json)
	target_link_libraries(gui_app PRIVATE plot_utils)
	target_link_libraries(gui_app PRIVATE plot_config)
	target_link_libraries(gui_app PRIVATE pointcloud_utils)
	target_link_libraries(gui_app PRIVATE string_utils)
	target_link_libraries(gui_app PRIVATE math_utils)
	target_link_libraries(gui_app PRIVATE wxCustomWidgets)
	target_link_libraries(gui_app PRIVATE tinyply)

	#
	# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
	# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
	set_property(TARGET gui_app PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

endif()

#******************************************************************************
# Installer
#******************************************************************************


