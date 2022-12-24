
project(CeresDataRepair VERSION 1.0.0.0)

#-------------------------------------------------------------------------------
# This executable will use the Qt GUI framework.
#-------------------------------------------------------------------------------
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/Resources/UI")


include("SoftwareInformation/branding.cmake")


set(ui_files
  Resources/UI/MainWindow.ui
  Resources/UI/AboutDialog.ui
)

if (MSVC)
  set(platform_resource_files
    Resources/CeresDataRepair.rc
)
endif()

set(resource_files
  CeresDataRepair.qrc
)

source_group("Resources" FILES
  ${ui_files}
  ${resource_files}
  ${platform_resource_files}
)

qt5_add_resources(UI_RESOURCES CeresDataRepair.qrc OPTIONS "--verbose")


add_executable(CeresDataRepair WIN32 MACOSX_BUNDLE)

set_target_properties(CeresDataRepair PROPERTIES LANGUAGE CXX)

target_compile_features(CeresDataRepair PRIVATE cxx_std_20)

target_sources(CeresDataRepair
PRIVATE

    ${ui_files}
    ${UI_RESOURCES}
    ${platform_resource_files}

	BlockDataFileExceptions.hpp
    BlockDataFile.hpp
	BlockId.hpp
	ClassIdentifiers.hpp
    DataBuffer.hpp
	RawDataBuffer.hpp
	BlockSerializer.hpp
	BlockParser.hpp

	BlockDataFile.cpp
    DataBuffer.cpp
    RawDataBuffer.cpp
	BlockSerializer.cpp
	BlockParser.cpp

	DataRepair.hpp
	DataRepair.cpp
	
	CentralWidget.hpp
	CentralWidget.cpp
	MainWindow.hpp
	MainWindow.cpp
	
	CeresDataRepair.cpp
)


target_include_directories(CeresDataRepair PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
target_include_directories(CeresDataRepair PRIVATE ${UTILITIES_INCLUDE_DIR})


target_link_libraries(CeresDataRepair PRIVATE ${Qt}::Widgets)
target_link_libraries(CeresDataRepair PRIVATE ${Qt}::Core)

target_link_libraries(CeresDataRepair PRIVATE Utilities)


#
# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
set_property(TARGET CeresDataRepair PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")


set_target_properties(CeresDataRepair PROPERTIES FOLDER "Apps")

install(TARGETS CeresDataRepair DESTINATION bin)

