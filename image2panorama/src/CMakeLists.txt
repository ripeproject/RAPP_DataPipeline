

set(IMAGE_TO_PANORAMA_COMMON_FILES

	ParserExceptions.hpp

	image2panorama.hpp
	image2panorama.cpp
	
	FileProcessor.hpp
	FileProcessor.cpp
)


add_executable(console_app)

set_target_properties(console_app PROPERTIES LANGUAGE CXX)
set_target_properties(console_app PROPERTIES OUTPUT_NAME "image2panorama")

target_compile_features(console_app PRIVATE cxx_std_20)

target_sources(console_app
PRIVATE

	BS_thread_pool.hpp

	${IMAGE_TO_PANORAMA_COMMON_FILES}

	ConsoleApp.cpp
)


target_include_directories(console_app PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
#target_include_directories(console_app PRIVATE "../support/PointCloud")
target_include_directories(console_app PRIVATE "../support/ProcessingInfo")
target_include_directories(console_app PRIVATE "../support/StringUtils")

target_link_libraries(console_app PRIVATE cbdf::cbdf)
target_link_libraries(console_app PRIVATE cbdf::info)
target_link_libraries(console_app PRIVATE cbdf::ctrl)
target_link_libraries(console_app PRIVATE cbdf::sensors)
#target_link_libraries(console_app PRIVATE pointcloud_io)
target_link_libraries(console_app PRIVATE processing_info)
target_link_libraries(console_app PRIVATE string_utils)


#
# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
set_property(TARGET console_app PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")


if(wxWidgets_FOUND)

	if(WIN32)
		# Include a RC file for windows
		set(resource_files
			./Resources/ripe.ico
			./Resources/ImagePanorama.rc
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

		${IMAGE_TO_PANORAMA_COMMON_FILES}
		${resource_files}
		
		GuiApp.cpp
	)

	include(${wxWidgets_USE_FILE})

	target_include_directories(gui_app PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
	#target_include_directories(gui_app PRIVATE "../support/PointCloud")
	target_include_directories(gui_app PRIVATE "../support/ProcessingInfo")
	target_include_directories(gui_app PRIVATE "../support/StringUtils")

	target_link_libraries(gui_app PRIVATE ${CMAKE_THREAD_LIBS_INIT})
	target_link_libraries(gui_app PRIVATE ${wxWidgets_LIBRARIES})
	target_link_libraries(gui_app PRIVATE cbdf::cbdf)
	target_link_libraries(gui_app PRIVATE cbdf::info)
	target_link_libraries(gui_app PRIVATE cbdf::ctrl)
	target_link_libraries(gui_app PRIVATE cbdf::sensors)
	#target_link_libraries(gui_app PRIVATE pointcloud_io)
	target_link_libraries(gui_app PRIVATE processing_info)
	target_link_libraries(gui_app PRIVATE string_utils)


	#
	# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
	# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
	set_property(TARGET gui_app PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

endif()



#******************************************************************************
# Installer
#******************************************************************************


