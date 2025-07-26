

# Create the version information code
configure_file(version.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/version.cpp @ONLY)


set(FILE_REPAIR_PUBLIC_HEADER_FILES

	ParserExceptions.hpp
	DataFileRecovery.hpp
)



# The static library (.lib/.a) for repairing Ceres based files
add_library(CeresFileRepair_static STATIC)

if (MSVC)
  # Add the "lib" prefix for generated .lib outputs.
	set_target_properties(CeresFileRepair_static PROPERTIES OUTPUT_NAME "libCeresFileRepair")
else ()
	set_target_properties(CeresFileRepair_static PROPERTIES OUTPUT_NAME "CeresFileRepair")
endif (MSVC)

set_target_properties(CeresFileRepair_static PROPERTIES LANGUAGE CXX)

target_compile_features(CeresFileRepair_static PRIVATE cxx_std_20)

#target_include_directories(FileVerificator_static PUBLIC
#	$<BUILD_INTERFACE:${LIBSPIDERCAM_INCLUDE_DIR}>
#	$<INSTALL_INTERFACE:include>
#)

target_sources(CeresFileRepair_static
PRIVATE
	${FILE_REPAIR_PUBLIC_HEADER_FILES}

PRIVATE
	BlockDataFileRecovery.hpp
	FileRepairProcessor.hpp

	BlockDataFileRecovery.cpp
	DataFileRecovery.cpp
	FileRepairProcessor.cpp
)

#
# Setting MSVC_STATIC_RUNTIME determines which MSVC runtime to use...
if (MSVC_STATIC_RUNTIME)
	# To use MultiThreaded (-MT) and MultiThreadedDebug (-MTd)
	set_property(TARGET CeresFileRepair_static PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else()
	# To use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
	set_property(TARGET CeresFileRepair_static PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()


add_executable(console_app)

set_target_properties(console_app PROPERTIES LANGUAGE CXX)
set_target_properties(console_app PROPERTIES OUTPUT_NAME "FileRepair")

target_compile_features(console_app PRIVATE cxx_std_20)

target_sources(console_app
PRIVATE
	BS_thread_pool.hpp
	
	ConsoleApp.cpp
)


target_include_directories(console_app PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
target_include_directories(console_app PRIVATE "../support/Utilities")
target_include_directories(console_app PRIVATE "../support/StringUtils")

target_link_libraries(console_app PRIVATE CeresFileRepair_static)
target_link_libraries(console_app PRIVATE cbdf::cbdf)
target_link_libraries(console_app PRIVATE string_utils)
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
			./Resources/CeresFileRepair.rc
		)

	#	source_group("Resources" FILES
	#		${resource_files}
	#)

	elseif(APPLE)

		# Add an icon for the apple .app file
		set(resource_files
			Resources/ripe.icns
		)

	endif()

	source_group("Resources" FILES
		${resource_files}
	)

	add_executable(gui_app WIN32 MACOSX_BUNDLE)

	set_target_properties(gui_app PROPERTIES LANGUAGE CXX)
	set_target_properties(gui_app PROPERTIES OUTPUT_NAME "CeresFileRepair")

	target_compile_features(gui_app PRIVATE cxx_std_20)

	target_sources(gui_app
	PRIVATE

		MainWindow.hpp
		MainWindow.cpp
		MainFrame.hpp
		MainFrame.cpp

		${resource_files}
		
#		version.hpp
#		version.cpp
		
		GuiApp.cpp
	)

	include(${wxWidgets_USE_FILE})

	target_include_directories(gui_app PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
	target_include_directories(gui_app PRIVATE "../support/StringUtils")
	target_include_directories(gui_app PRIVATE "../support/wxCustomWidgets")

	target_link_libraries(gui_app PRIVATE ${CMAKE_THREAD_LIBS_INIT})
	target_link_libraries(gui_app PRIVATE ${wxWidgets_LIBRARIES})
	target_link_libraries(gui_app PRIVATE CeresFileRepair_static)
	target_link_libraries(gui_app PRIVATE cbdf::cbdf)
	target_link_libraries(gui_app PRIVATE string_utils)
	target_link_libraries(gui_app PRIVATE wxCustomWidgets)

	#
	# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
	# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
	set_property(TARGET gui_app PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

endif()



#******************************************************************************
# Installer
#******************************************************************************


install(TARGETS CeresFileRepair_static
	EXPORT ${PROJECT_NAME}-config
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(EXPORT ${PROJECT_NAME}-config
	NAMESPACE ${PROJECT_NAME}::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(FILES ${FILE_REPAIR_PUBLIC_HEADER_FILES}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})



