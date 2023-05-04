


set(PLOT_SPLIT_COMMON_FILES

	ParserExceptions.hpp

	PlotBoundaries.hpp
	PlotBoundaries.cpp

	FileProcessor.hpp
	FileProcessor.cpp
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

target_link_libraries(console_app PRIVATE cbdf::cbdf)
target_link_libraries(console_app PRIVATE cbdf::info)

#
# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
set_property(TARGET console_app PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")


if(wxWidgets_FOUND)

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
		
		GuiApp.cpp
	)

	include(${wxWidgets_USE_FILE})

	target_include_directories(gui_app PRIVATE ${CMAKE_INSTALL_PREFIX}/include)

	target_link_libraries(gui_app PRIVATE ${CMAKE_THREAD_LIBS_INIT})
	target_link_libraries(gui_app PRIVATE ${wxWidgets_LIBRARIES})
	target_link_libraries(gui_app PRIVATE cbdf::cbdf)
	target_link_libraries(gui_app PRIVATE cbdf::info)

	#
	# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
	# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
	set_property(TARGET gui_app PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

endif()

#******************************************************************************
# Installer
#******************************************************************************


