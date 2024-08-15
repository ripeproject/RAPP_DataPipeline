

add_subdirectory(bdf_v1)


# Create the version information code
configure_file(version.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/version.cpp @ONLY)


set(FILE_VERIFICATOR_COMMON_FILES

	ParserExceptions.hpp

	LidarFileVerifier.hpp
	LidarFileVerifier.cpp

	CeresFileVerifier.hpp
	CeresFileVerifier.cpp

	AbstractFileVerifier.hpp
)


add_executable(console_app)

set_target_properties(console_app PROPERTIES LANGUAGE CXX)
set_target_properties(console_app PROPERTIES OUTPUT_NAME "FileChecker")

target_compile_features(console_app PRIVATE cxx_std_20)

target_sources(console_app
PRIVATE

	BS_thread_pool.hpp

	${FILE_VERIFICATOR_COMMON_FILES}

	ConsoleApp.cpp
)


target_include_directories(console_app PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
target_include_directories(console_app PRIVATE "../support/Utilities")
target_include_directories(console_app PRIVATE "../support/StringUtils")

target_link_libraries(console_app PRIVATE BlockDataFile_v1)
target_link_libraries(console_app PRIVATE cbdf::cbdf)
target_link_libraries(console_app PRIVATE string_utils)
target_link_libraries(console_app PRIVATE utilities)

set_property(TARGET console_app PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")


if(wxWidgets_FOUND)

	if(WIN32)
		# Include a RC file for windows
		set(resource_files
			./Resources/ripe.ico
			./Resources/CeresFileVerification.rc
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
	set_target_properties(gui_app PROPERTIES OUTPUT_NAME "CeresFileChecker")

	target_compile_features(gui_app PRIVATE cxx_std_20)

	target_sources(gui_app
	PRIVATE

		MainWindow.hpp
		MainWindow.cpp
		MainFrame.hpp
		MainFrame.cpp

		${FILE_VERIFICATOR_COMMON_FILES}

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
	target_link_libraries(gui_app PRIVATE string_utils)
	target_link_libraries(gui_app PRIVATE wxCustomWidgets)

	set_property(TARGET gui_app PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

endif()

if(UNIX AND NOT APPLE)
	add_executable(service)

	set_target_properties(service PROPERTIES LANGUAGE CXX)
	set_target_properties(service PROPERTIES OUTPUT_NAME "ceres_file_verifier_d")

	target_compile_features(service PRIVATE cxx_std_20)

	target_sources(service
	PRIVATE

		BS_thread_pool.hpp

		${FILE_VERIFICATOR_COMMON_FILES}

		Service_Linux.cpp
	)


	target_include_directories(service PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
	target_include_directories(service PRIVATE "../support/Utilities")
	target_include_directories(service PRIVATE "../support/StringUtils")

	target_link_libraries(service PRIVATE BlockDataFile_v1)
	target_link_libraries(service PRIVATE cbdf::cbdf)
	target_link_libraries(service PRIVATE string_utils)
	target_link_libraries(service PRIVATE utilities)
endif()

if(WIN32)
	add_executable(service)

	set_target_properties(service PROPERTIES LANGUAGE CXX)
	set_target_properties(service PROPERTIES OUTPUT_NAME "FileCheckerService")

	target_compile_features(service PRIVATE cxx_std_20)

	target_sources(service
	PRIVATE

		BS_thread_pool.hpp

		${FILE_VERIFICATOR_COMMON_FILES}

		Service_Windows.cpp
	)


	target_include_directories(service PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
	target_include_directories(service PRIVATE "../support/Utilities")
	target_include_directories(service PRIVATE "../support/StringUtils")

	target_link_libraries(service PRIVATE BlockDataFile_v1)
	target_link_libraries(service PRIVATE cbdf::cbdf)
	target_link_libraries(service PRIVATE string_utils)
	target_link_libraries(service PRIVATE utilities)
endif()


#******************************************************************************
# Installer
#******************************************************************************


