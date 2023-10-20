

#-------------------------------------------------------------------------------
# Creating a data storage support library
#-------------------------------------------------------------------------------

add_library(BlockDataFile_v1 STATIC)

set_target_properties(BlockDataFile_v1 PROPERTIES LANGUAGE CXX)

target_compile_features(BlockDataFile_v1 PRIVATE cxx_std_20)

target_sources(BlockDataFile_v1
PUBLIC

	BlockDataFileExceptions.hpp
	BlockDataFile.hpp
	ClassIdentifiers.hpp
	BlockId.hpp
    DataBuffer.hpp
	RawDataBuffer.hpp
	BlockSerializer.hpp
	BlockParser.hpp

	PvtBlockId.hpp
	PvtDataIdentifiers.hpp
	PvtSerializer.hpp
	PvtParser.hpp

    OusterBlockId.hpp
    OusterDataIdentifiers.hpp
    OusterSerializer.hpp
    OusterParser.hpp
	
	ParserExceptions.hpp

PRIVATE

	BlockDataFile.cpp
    DataBuffer.cpp
    RawDataBuffer.cpp
	BlockSerializer.cpp
	BlockParser.cpp

	PvtBlockId.cpp
	PvtSerializer.cpp
	PvtParser.cpp
	
    OusterBlockId.cpp
	OusterSerializer.cpp
	OusterParser.cpp

	Crc.hpp
	Crc.cpp
)

target_include_directories(BlockDataFile_v1 PRIVATE ${CMAKE_INSTALL_PREFIX}/include)

target_link_libraries(BlockDataFile_v1 PUBLIC ouster::ouster_static)

#
# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
set_property(TARGET BlockDataFile_v1 PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

