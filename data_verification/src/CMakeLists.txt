

add_subdirectory(bdf_v1)


# Create the version information code
configure_file(version.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/version.cpp @ONLY)


set(DATA_VERIFICATOR_COMMON_FILES

	ParserExceptions.hpp

	OusterTypes.hpp
	OusterVerificationParser.hpp
	OusterVerificationParser.cpp
	AxisCommunicationsVerificationParser.hpp
	AxisCommunicationsVerificationParser.cpp

	LidarDataVerifier.hpp
	LidarDataVerifier.cpp

	CeresDataVerifier.hpp
	CeresDataVerifier.cpp
)


add_executable(console_app)

set_target_properties(console_app PROPERTIES LANGUAGE CXX)
set_target_properties(console_app PROPERTIES OUTPUT_NAME "DataChecker")

target_compile_features(console_app PRIVATE cxx_std_20)

target_sources(console_app
PRIVATE

	BS_thread_pool.hpp

	${DATA_VERIFICATOR_COMMON_FILES}

	ConsoleApp.cpp
)


target_include_directories(console_app PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
target_include_directories(console_app PRIVATE "../support/Utilities")
target_include_directories(console_app PRIVATE "../support/StringUtils")

target_link_libraries(console_app PRIVATE BlockDataFile_v1)
target_link_libraries(console_app PRIVATE cbdf::cbdf)
target_link_libraries(console_app PRIVATE cbdf::lidar cbdf::rgb)
target_link_libraries(console_app PRIVATE string_utils)
target_link_libraries(console_app PRIVATE utilities)

set_property(TARGET console_app PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")


if(wxWidgets_FOUND)

	if(WIN32)
		# Include a RC file for windows
		set(resource_files
			./Resources/ripe.ico
			./Resources/CeresDataVerification.rc
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
	set_target_properties(gui_app PROPERTIES OUTPUT_NAME "CeresDataChecker")

	target_compile_features(gui_app PRIVATE cxx_std_20)

	target_sources(gui_app
	PRIVATE

		MainWindow.hpp
		MainWindow.cpp
		MainFrame.hpp
		MainFrame.cpp

		${DATA_VERIFICATOR_COMMON_FILES}

		${resource_files}
		
#		version.cpp
		
		GuiApp.cpp
	)

	include(${wxWidgets_USE_FILE})

	target_include_directories(gui_app PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
	target_include_directories(gui_app PRIVATE "../support/StringUtils")
	target_include_directories(gui_app PRIVATE "../support/wxCustomWidgets")

	target_link_libraries(gui_app PRIVATE ${CMAKE_THREAD_LIBS_INIT})
	target_link_libraries(gui_app PRIVATE ${wxWidgets_LIBRARIES})
	target_link_libraries(gui_app PRIVATE BlockDataFile_v1)
	target_link_libraries(gui_app PRIVATE cbdf::cbdf)
	target_link_libraries(gui_app PRIVATE cbdf::lidar cbdf::rgb)
	target_link_libraries(gui_app PRIVATE string_utils)
	target_link_libraries(gui_app PRIVATE wxCustomWidgets)

	set_property(TARGET gui_app PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

endif()



#******************************************************************************
# Installer
#******************************************************************************


