

#******************************************************************************
# APPLICATION VERSION INFORMATION
#******************************************************************************


# Use integer versions instead of strings for easier handling if required
SET(pointcloud2ply_VERSION_MAJOR 1)
SET(pointcloud2ply_VERSION_MINOR 4)
SET(pointcloud2ply_VERSION_PATCH 0)
SET(pointcloud2ply_VERSION_TWEAK 0)

ADD_DEFINITIONS(
     -DPOINTCLOUD_2_PLY_MAJOR=$pointcloud2ply_VERSION_MAJOR}
     -DPOINTCLOUD_2_PLY_MINOR=$pointcloud2ply_VERSION_MINOR}
     -DPOINTCLOUD_2_PLY_PATCH=$pointcloud2ply_VERSION_PATCH}
     -DPOINTCLOUD_2_PLY_TWEAK=$pointcloud2ply_VERSION_TWEAK}
)

SET(pointcloud2ply_VERSION "${pointcloud2ply_VERSION_MAJOR}.${pointcloud2ply_VERSION_MINOR}.${pointcloud2ply_VERSION_PATCH}.${pointcloud2ply_VERSION_TWEAK}")

ADD_DEFINITIONS(-DPOINTCLOUD_2_PLY_VERSION="${pointcloud2ply_VERSION}")


# Configure version resource file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Export2Ply.rc.in
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Export2Ply.rc
    @ONLY
)

# Create the version information code
include_directories(${CMAKE_CURRENT_BINARY_DIR})
configure_file(version.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/version.cpp @ONLY)


#******************************************************************************
# APPLICATION
#******************************************************************************

set(POINT_CLOUD_2_PLY_COMMON_FILES

	ParserExceptions.hpp

	pointcloud2ply.hpp
	pointcloud2ply.cpp

	FileProcessor.hpp
	FileProcessor.cpp
	
	version.hpp
	${CMAKE_CURRENT_BINARY_DIR}/version.cpp
)


add_executable(console_app)

set_target_properties(console_app PROPERTIES LANGUAGE CXX)
set_target_properties(console_app PROPERTIES OUTPUT_NAME "pointcloud2ply")

target_compile_features(console_app PRIVATE cxx_std_20)

target_sources(console_app
PRIVATE

	BS_thread_pool.hpp

	${POINT_CLOUD_2_PLY_COMMON_FILES}

	ConsoleApp.cpp
)


target_include_directories(console_app PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
target_include_directories(console_app PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(console_app PRIVATE "../support/common")
target_include_directories(console_app PRIVATE "../support/StringUtils")
target_include_directories(console_app PRIVATE "../support/Utilities")

target_link_libraries(console_app PRIVATE cbdf::cbdf)
target_link_libraries(console_app PRIVATE cbdf::ctrl)
target_link_libraries(console_app PRIVATE cbdf::plot_info)
target_link_libraries(console_app PRIVATE cbdf::pointcloud)
target_link_libraries(console_app PRIVATE string_utils)
target_link_libraries(console_app PRIVATE tinyply)
target_link_libraries(console_app PRIVATE utilities)


#
# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
set_property(TARGET console_app PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")


if(wxWidgets_FOUND)

	if(WIN32)
		# Include a RC file for windows
		set(resource_files
			./Resources/ripe.ico
			./Resources/Export2Ply.rc
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
	set_target_properties(gui_app PROPERTIES OUTPUT_NAME "Export2Ply")

	target_compile_features(gui_app PRIVATE cxx_std_20)

	target_sources(gui_app
	PRIVATE

		MainWindow.hpp
		MainWindow.cpp
		MainFrame.hpp
		MainFrame.cpp

		${POINT_CLOUD_2_PLY_COMMON_FILES}

		${resource_files}
		
		GuiApp.cpp
	)

	include(${wxWidgets_USE_FILE})

	target_include_directories(gui_app PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
	target_include_directories(gui_app PRIVATE ${CMAKE_CURRENT_LIST_DIR})
	target_include_directories(gui_app PRIVATE "../support/common")
	target_include_directories(gui_app PRIVATE "../support/StringUtils")
	target_include_directories(gui_app PRIVATE "../support/wxCustomWidgets")

	target_link_libraries(gui_app PRIVATE ${CMAKE_THREAD_LIBS_INIT})
	target_link_libraries(gui_app PRIVATE ${wxWidgets_LIBRARIES})
	target_link_libraries(gui_app PRIVATE cbdf::cbdf)
	target_link_libraries(gui_app PRIVATE cbdf::ctrl)
	target_link_libraries(gui_app PRIVATE cbdf::pointcloud)
	target_link_libraries(gui_app PRIVATE cbdf::plot_info)
	target_link_libraries(gui_app PRIVATE string_utils)
	target_link_libraries(gui_app PRIVATE tinyply)
	target_link_libraries(gui_app PRIVATE wxCustomWidgets)

	#
	# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
	# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
	set_property(TARGET gui_app PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

endif()



#******************************************************************************
# Installer
#******************************************************************************


