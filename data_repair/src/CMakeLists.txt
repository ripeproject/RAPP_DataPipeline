


add_executable(DataRepair)

set_target_properties(DataRepair PROPERTIES LANGUAGE CXX)

target_compile_features(DataRepair PRIVATE cxx_std_20)

target_sources(DataRepair
PRIVATE

	BS_thread_pool.hpp

	OusterTypes.hpp
	OusterVerificationParser.hpp
	OusterVerificationParser.cpp
	AxisCommunicationsVerificationParser.hpp
	AxisCommunicationsVerificationParser.cpp
	ParserExceptions.hpp

	DataRepair.hpp
	DataRepair.cpp
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


