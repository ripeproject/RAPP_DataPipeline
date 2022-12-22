


add_executable(CeresDataVerification)

set_target_properties(CeresDataVerification PROPERTIES LANGUAGE CXX)

target_compile_features(CeresDataVerification PRIVATE cxx_std_20)

target_sources(CeresDataVerification
PRIVATE

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


target_include_directories(CeresDataVerification PRIVATE ${CMAKE_INSTALL_PREFIX}/include)

target_link_libraries(CeresDataVerification PRIVATE cbdf::cbdf)
target_link_libraries(CeresDataVerification PRIVATE cbdf::sensors)

#
# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
set_property(TARGET CeresDataVerification PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")


#******************************************************************************
# Installer
#******************************************************************************


