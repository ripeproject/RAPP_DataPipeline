


add_executable(DataRepair)

set_target_properties(DataRepair PROPERTIES LANGUAGE CXX)

target_compile_features(DataRepair PRIVATE cxx_std_20)

target_sources(DataRepair
PRIVATE

	BS_thread_pool.hpp
	ParserExceptions.hpp

	AxisCommunicationsRepairParser.hpp
	AxisCommunicationsRepairParser.cpp

	OusterRepairParser.hpp
	OusterRepairParser.cpp

	DataRepair.hpp
	DataRepair.cpp

	BlockDataFileRecovery.hpp
	BlockDataFileRecovery.cpp

	DataFileRecovery.hpp
	DataFileRecovery.cpp

	FileProcessor.hpp
	FileProcessor.cpp

	CeresDataRepair.cpp
)


target_include_directories(DataRepair PRIVATE ${CMAKE_INSTALL_PREFIX}/include)

target_link_libraries(DataRepair PRIVATE cbdf::cbdf)
target_link_libraries(DataRepair PRIVATE cbdf::sensors)

#
# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
set_property(TARGET DataRepair PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")


#******************************************************************************
# Installer
#******************************************************************************


