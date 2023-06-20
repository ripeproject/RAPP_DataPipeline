


add_executable(carl)

set_target_properties(carl PROPERTIES LANGUAGE CXX)

target_compile_features(carl PRIVATE cxx_std_20)

target_sources(carl
PRIVATE

	main.cpp
)


target_link_libraries(carl PRIVATE fmt::fmt)

#
# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
set_property(TARGET carl PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")


#******************************************************************************
# Installer
#******************************************************************************


