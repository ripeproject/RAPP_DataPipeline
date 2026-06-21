

# Configure version resource file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources/DailyChecker.rc.in
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources/DailyChecker.rc
    @ONLY
)

# Create the version information code
configure_file(version.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/version.cpp @ONLY)

if(WIN32)
	# Include a RC file for windows
	set(daily_checker_resource_files
		./Resources/DailyChecker.rc
	)

	source_group("Resources" FILES
		${daily_checker_resource_files}
	)

elseif(APPLE)

	# Add an icon for the apple .app file
	set(daily_checker_resource_files
		Resources/ripe.icns
	)

endif()


add_executable(console_app)

set_target_properties(console_app PROPERTIES LANGUAGE CXX)
set_target_properties(console_app PROPERTIES OUTPUT_NAME "DailyChecker")

target_compile_features(console_app PRIVATE cxx_std_20)

target_sources(console_app
PRIVATE

	BS_thread_pool.hpp

	CeresDailyChecker.hpp
	CeresDailyChecker.cpp

	${daily_checker_resource_files}

	ConsoleApp.cpp
)


target_include_directories(console_app PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
target_include_directories(console_app PRIVATE "../support/Utilities")
target_include_directories(console_app PRIVATE "../support/StringUtils")

target_link_libraries(console_app PRIVATE file_verificator::CeresFileVerifier_static)
target_link_libraries(console_app PRIVATE file_repair::CeresFileRepair_static)
target_link_libraries(console_app PRIVATE data_verificator::CeresDataVerifier_static)
target_link_libraries(console_app PRIVATE data_repair::CeresDataRepair_static)
target_link_libraries(console_app PRIVATE cbdf::cbdf)
target_link_libraries(console_app PRIVATE cbdf::info)
target_link_libraries(console_app PRIVATE cbdf::gps cbdf::lidar cbdf::rgb cbdf::hyperspectral cbdf::ir)
target_link_libraries(console_app PRIVATE string_utils)
target_link_libraries(console_app PRIVATE utilities)

set_property(TARGET console_app PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")


if(wxWidgets_FOUND)

	# Configure version resource file
	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/Resources/CeresDailyChecker.rc.in
		${CMAKE_CURRENT_SOURCE_DIR}/Resources/CeresDailyChecker.rc
		@ONLY
	)

	if(WIN32)
		# Include a RC file for windows
		set(ceres_daily_checker_resource_files
			./Resources/ripe.ico
			./Resources/CeresDailyChecker.rc
		)

		source_group("Resources" FILES
			${ceres_daily_checker_resource_files}
		)

	elseif(APPLE)

		# Add an icon for the apple .app file
		set(ceres_daily_checker_resource_files
			Resources/ripe.icns
		)

	endif()

	add_executable(gui_app WIN32 MACOSX_BUNDLE)

	set_target_properties(gui_app PROPERTIES LANGUAGE CXX)
	set_target_properties(gui_app PROPERTIES OUTPUT_NAME "CeresDailyChecker")

	target_compile_features(gui_app PRIVATE cxx_std_20)

	target_sources(gui_app
	PRIVATE
		ResultsDlg.hpp
		ResultsDlg.cpp

		MainWindow.hpp
		MainWindow.cpp
		MainFrame.hpp
		MainFrame.cpp

		CeresDailyChecker.hpp
		CeresDailyChecker.cpp

		${ceres_daily_checker_resource_files}
		
#		version.cpp
		
		GuiApp.cpp
	)

	get_source_file_property(defs MainFrame.cpp COMPILE_DEFINITIONS)
		list(APPEND defs "CERES_DAILY_CHECKER_VERSION=\"${daily_checker_VERSION}\"")
		list(APPEND defs "CERES_DAILY_CHECKER_MAJOR_VERSION=\"${daily_checker_VERSION_MAJOR}\"")
		list(APPEND defs "CERES_DAILY_CHECKER_MINOR_VERSION=\"${daily_checker_VERSION_MINOR}\"")
		set_source_files_properties(MainFrame.cpp PROPERTIES
			COMPILE_DEFINITIONS "${defs}"
	)

	include(${wxWidgets_USE_FILE})

	target_include_directories(gui_app PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
	target_include_directories(gui_app PRIVATE "../support/Utilities")
	target_include_directories(gui_app PRIVATE "../support/StringUtils")
	target_include_directories(gui_app PRIVATE "../support/wxCustomWidgets")

	target_link_libraries(gui_app PRIVATE ${CMAKE_THREAD_LIBS_INIT})
	target_link_libraries(gui_app PRIVATE ${wxWidgets_LIBRARIES})
	target_link_libraries(gui_app PRIVATE file_verificator::CeresFileVerifier_static)
	target_link_libraries(gui_app PRIVATE file_repair::CeresFileRepair_static)
	target_link_libraries(gui_app PRIVATE data_verificator::CeresDataVerifier_static)
	target_link_libraries(gui_app PRIVATE data_repair::CeresDataRepair_static)
	target_link_libraries(gui_app PRIVATE cbdf::cbdf)
	target_link_libraries(gui_app PRIVATE cbdf::info)
	target_link_libraries(gui_app PRIVATE cbdf::gps cbdf::lidar cbdf::rgb cbdf::hyperspectral cbdf::ir)
	target_link_libraries(gui_app PRIVATE string_utils)
	target_link_libraries(gui_app PRIVATE utilities)
	target_link_libraries(gui_app PRIVATE wxCustomWidgets)

	set_property(TARGET gui_app PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

endif()



#******************************************************************************
# Installer
#******************************************************************************




