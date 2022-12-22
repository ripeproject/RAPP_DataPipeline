


add_executable(DataVerificator)

set_target_properties(DataVerificator PROPERTIES LANGUAGE CXX)

target_compile_features(DataVerificator PRIVATE cxx_std_20)

target_sources(DataVerificator
PRIVATE

	BS_thread_pool.hpp

	OusterTypes.hpp
	OusterVerificationParser.hpp
	OusterVerificationParser.cpp
	AxisCommunicationsVerificationParser.hpp
	AxisCommunicationsVerificationParser.cpp
	ParserExceptions.hpp

	DataVerifier.hpp
	DataVerifier.cpp
	CeresDataVerification.cpp
)


target_include_directories(DataVerificator PRIVATE ${CMAKE_INSTALL_PREFIX}/include)

target_link_libraries(DataVerificator PRIVATE cbdf::cbdf)
target_link_libraries(DataVerificator PRIVATE cbdf::sensors)

#
# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
set_property(TARGET DataVerificator PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")


#******************************************************************************
# Installer
#******************************************************************************


