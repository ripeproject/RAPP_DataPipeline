#******************************************************************************
# APPLICATION VERSION INFORMATION
#******************************************************************************


# Use integer versions instead of strings for easier handling if required
SET(export_jpegs_VERSION_MAJOR 1)
SET(export_jpegs_VERSION_MINOR 3)
SET(export_jpegs_VERSION_PATCH 0)
SET(export_jpegs_VERSION_TWEAK 1)

ADD_DEFINITIONS(
     -DEXPORT_JPEGS_MAJOR=${export_jpegs_VERSION_MAJOR}
     -DEXPORT_JPEGS_MINOR=${export_jpegs_VERSION_MINOR}
     -DEXPORT_JPEGS_PATCH=${export_jpegs_VERSION_PATCH}
     -DEXPORT_JPEGS_TWEAK=${export_jpegs_VERSION_TWEAK}
)

SET(export_jpegs_VERSION "${export_jpegs_VERSION_MAJOR}.${export_jpegs_VERSION_MINOR}.${export_jpegs_VERSION_PATCH}.${export_jpegs_VERSION_TWEAK}")

ADD_DEFINITIONS(-DEXPORT_JPEGS_VERSION="${export_jpegs_VERSION}")


# Configure version resource file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources/ExportJpegs.rc.in
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources/ExportJpegs.rc
    @ONLY
)

# Create the version information code
include_directories(${CMAKE_CURRENT_BINARY_DIR})
configure_file(version.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/version.cpp @ONLY)


#******************************************************************************
# APPLICATION
#******************************************************************************

set(EXPORT_JPEGS_COMMON_FILES

	ParserExceptions.hpp

	export_jpegs.hpp
	export_jpegs.cpp

	FileProcessor.hpp
	FileProcessor.cpp
	
	version.hpp
	${CMAKE_CURRENT_BINARY_DIR}/version.cpp
)


add_executable(console_app)

set_target_properties(console_app PROPERTIES LANGUAGE CXX)
set_target_properties(console_app PROPERTIES OUTPUT_NAME "export_jpegs")

target_compile_features(console_app PRIVATE cxx_std_20)

target_sources(console_app
PRIVATE

	BS_thread_pool.hpp

	${EXPORT_JPEGS_COMMON_FILES}

	ConsoleApp.cpp
)


target_include_directories(console_app PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
target_include_directories(console_app PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(console_app PRIVATE "../support/StringUtils")
target_include_directories(console_app PRIVATE "../support/FieldUtils")
target_include_directories(console_app PRIVATE "../support/common")
target_include_directories(console_app PRIVATE "../support/PlotConfig")
target_include_directories(console_app PRIVATE "../support/PlotUtils")

target_link_libraries(console_app PRIVATE Exiv2::exiv2lib)
target_link_libraries(console_app PRIVATE cbdf::cbdf)
target_link_libraries(console_app PRIVATE cbdf::info cbdf::ctrl cbdf::rgb cbdf::gps)
target_link_libraries(console_app PRIVATE string_utils)
target_link_libraries(console_app PRIVATE field_utils)
target_link_libraries(console_app PRIVATE plot_config)


#
# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
set_property(TARGET console_app PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")


if(wxWidgets_FOUND)

	if(WIN32)
		# Include a RC file for windows
		set(resource_files
			./Resources/ripe.ico
			./Resources/ExportJpegs.rc
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
	set_target_properties(gui_app PROPERTIES OUTPUT_NAME "Export2Jpeg")

	target_compile_features(gui_app PRIVATE cxx_std_20)

	target_sources(gui_app
	PRIVATE

		MainWindow.hpp
		MainWindow.cpp
		MainFrame.hpp
		MainFrame.cpp

		${EXPORT_JPEGS_COMMON_FILES}
		${resource_files}
		
		GuiApp.cpp
	)

	include(${wxWidgets_USE_FILE})

	target_include_directories(gui_app PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
	target_include_directories(gui_app PRIVATE ${CMAKE_CURRENT_LIST_DIR})
	target_include_directories(gui_app PRIVATE "../support/StringUtils")
	target_include_directories(gui_app PRIVATE "../support/FieldUtils")
	target_include_directories(gui_app PRIVATE "../support/common")
	target_include_directories(gui_app PRIVATE "../support/PlotConfig")
	target_include_directories(gui_app PRIVATE "../support/PlotUtils")

	target_link_libraries(gui_app PRIVATE ${CMAKE_THREAD_LIBS_INIT})
	target_link_libraries(gui_app PRIVATE ${wxWidgets_LIBRARIES})
	target_link_libraries(gui_app PRIVATE Exiv2::exiv2lib)
	target_link_libraries(gui_app PRIVATE cbdf::cbdf)
	target_link_libraries(gui_app PRIVATE cbdf::info cbdf::ctrl cbdf::rgb cbdf::gps)
	target_link_libraries(gui_app PRIVATE string_utils)
	target_link_libraries(gui_app PRIVATE field_utils)
	target_link_libraries(gui_app PRIVATE plot_config)
	target_link_libraries(gui_app PRIVATE wxCustomWidgets)

	#
	# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
	# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
	set_property(TARGET gui_app PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

endif()



#******************************************************************************
# Installer
#******************************************************************************


