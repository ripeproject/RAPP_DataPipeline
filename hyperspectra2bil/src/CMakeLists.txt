

set(HYPER_SPECTRA_2_BIL_COMMON_FILES

	ParserExceptions.hpp

	HySpexVNIR3000N_File.hpp
	HySpexVNIR3000N_File.cpp

	HySpexVNIR3000N_BIL.hpp
	HySpexVNIR3000N_BIL.cpp

	HySpexVNIR3000N_BIL_ENVI.hpp
	HySpexVNIR3000N_BIL_ENVI.cpp

	HySpexVNIR3000N_BIL_ArcMap.hpp
	HySpexVNIR3000N_BIL_ArcMap.cpp

	HySpexVNIR3000N_BIP.hpp
	HySpexVNIR3000N_BIP.cpp

	HySpexVNIR3000N_BIP_ENVI.hpp
	HySpexVNIR3000N_BIP_ENVI.cpp

	HySpexVNIR3000N_BIP_ArcMap.hpp
	HySpexVNIR3000N_BIP_ArcMap.cpp

	HySpexVNIR3000N_BSQ.hpp
	HySpexVNIR3000N_BSQ.cpp

	HySpexVNIR3000N_BSQ_ENVI.hpp
	HySpexVNIR3000N_BSQ_ENVI.cpp

	HySpexVNIR3000N_BSQ_ArcMap.hpp
	HySpexVNIR3000N_BSQ_ArcMap.cpp

	HySpexSWIR384_File.hpp
	HySpexSWIR384_File.cpp

	HySpexSWIR384_BIL.hpp
	HySpexSWIR384_BIL.cpp

	HySpexSWIR384_BIL_ENVI.hpp
	HySpexSWIR384_BIL_ENVI.cpp

	HySpexSWIR384_BIL_ArcMap.hpp
	HySpexSWIR384_BIL_ArcMap.cpp

	HySpexSWIR384_BIP.hpp
	HySpexSWIR384_BIP.cpp

	HySpexSWIR384_BIP_ENVI.hpp
	HySpexSWIR384_BIP_ENVI.cpp

	HySpexSWIR384_BIP_ArcMap.hpp
	HySpexSWIR384_BIP_ArcMap.cpp

	HySpexSWIR384_BSQ.hpp
	HySpexSWIR384_BSQ.cpp

	HySpexSWIR384_BSQ_ENVI.hpp
	HySpexSWIR384_BSQ_ENVI.cpp

	HySpexSWIR384_BSQ_ArcMap.hpp
	HySpexSWIR384_BSQ_ArcMap.cpp

	FileProcessor.hpp
	FileProcessor.cpp
)


add_executable(console_app)

set_target_properties(console_app PROPERTIES LANGUAGE CXX)
set_target_properties(console_app PROPERTIES OUTPUT_NAME "hyperspectra2bil")

target_compile_features(console_app PRIVATE cxx_std_20)

target_sources(console_app
PRIVATE

	BS_thread_pool.hpp

	${HYPER_SPECTRA_2_BIL_COMMON_FILES}

	ConsoleApp.cpp
)


target_include_directories(console_app PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
target_include_directories(console_app PRIVATE "../support/StringUtils")

target_link_libraries(console_app PRIVATE cbdf::cbdf)
target_link_libraries(console_app PRIVATE cbdf::info cbdf::ctrl)
target_link_libraries(console_app PRIVATE cbdf::hyperspectral)
target_link_libraries(console_app PRIVATE string_utils)
target_link_libraries(console_app PRIVATE hyspex_connect::hyspex_connect_minimal_static)
target_link_libraries(console_app PRIVATE ${OpenCV_LIBS} )


#
# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
set_property(TARGET console_app PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")


if(wxWidgets_FOUND)

	if(WIN32)
		# Include a RC file for windows
		set(resource_files
			./Resources/ripe.ico
			./Resources/Export2Bil.rc
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
	set_target_properties(gui_app PROPERTIES OUTPUT_NAME "Export2Bil")

	target_compile_features(gui_app PRIVATE cxx_std_20)

	target_sources(gui_app
	PRIVATE

		MainWindow.hpp
		MainWindow.cpp
		MainFrame.hpp
		MainFrame.cpp

		${HYPER_SPECTRA_2_BIL_COMMON_FILES}
		${resource_files}
		
		GuiApp.cpp
	)

	include(${wxWidgets_USE_FILE})

	target_include_directories(gui_app PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
	target_include_directories(gui_app PRIVATE "../support/StringUtils")
	target_include_directories(gui_app PRIVATE "../support/wxCustomWidgets")

	target_link_libraries(gui_app PRIVATE ${CMAKE_THREAD_LIBS_INIT})
	target_link_libraries(gui_app PRIVATE ${wxWidgets_LIBRARIES})
	target_link_libraries(gui_app PRIVATE cbdf::cbdf)
	target_link_libraries(gui_app PRIVATE cbdf::info cbdf::ctrl)
	target_link_libraries(gui_app PRIVATE cbdf::hyperspectral)
	target_link_libraries(gui_app PRIVATE string_utils)
	target_link_libraries(gui_app PRIVATE wxCustomWidgets)
	target_link_libraries(gui_app PRIVATE hyspex_connect::hyspex_connect_minimal_static)
	target_link_libraries(gui_app PRIVATE ${OpenCV_LIBS} )

	#
	# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
	# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
	set_property(TARGET gui_app PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

endif()



#******************************************************************************
# Installer
#******************************************************************************


