# If G2O is available, compile pose graph optimization stuff
if (g2o_FOUND)
  set(SLAM_g2o_sources src/PoseGraphOptimizer.cxx)
  set(g2o_targets g2o::core g2o::types_slam3d g2o::types_slam3d_addons)
  add_definitions(-DUSE_G2O=1)
endif()

# Generate export symbols on Windows to use this lib
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(LidarSlam
  CeresCostFunctions.h
  ConfidenceEstimators.h
  ConfidenceEstimators.cxx
  Enums.h
  KDTreePCLAdaptor.h
  KeypointsMatcher.h
  KeypointsMatcher.cxx
  LidarPoint.h
  LocalOptimizer.h
  LocalOptimizer.cxx
  MotionModel.h
  MotionModel.cxx
  PoseGraphOptimizer.h
  PoseGraphOptimizer.cxx
  RollingGrid.h
  RollingGrid.cxx
  ExternalSensorManagers.h
  ExternalSensorManagers.cxx
  Slam.h
  Slam.cxx
  SpinningSensorKeypointExtractor.h
  SpinningSensorKeypointExtractor.cxx
  Utilities.h
  Utilities.cxx
  VoxelGrid.h
  VoxelGrid.cxx
  ${SLAM_g2o_sources}
)

target_link_libraries(LidarSlam
  PUBLIC
    nanoflann::nanoflann
    ceres
    ${PCL_LIBRARIES}
    ${g2o_targets}
  PRIVATE
    Eigen3::Eigen
    ${OpenMP_target}
)

target_include_directories(LidarSlam PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)



add_executable(slam)

set_target_properties(slam PROPERTIES LANGUAGE CXX)

target_compile_features(slam PRIVATE cxx_std_20)

target_sources(slam
PRIVATE

	BS_thread_pool.hpp
	ParserExceptions.hpp

	pointcloud2slam.hpp
	pointcloud2slam.cpp

	FileProcessor.hpp
	FileProcessor.cpp
	main.cpp
)


target_include_directories(slam PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
target_include_directories(slam PRIVATE "../support/PointCloud")

target_link_libraries(slam PRIVATE cbdf::cbdf)
target_link_libraries(slam PRIVATE cbdf::ctrl)
target_link_libraries(slam PRIVATE pointcloud_io)
target_link_libraries(slam PRIVATE LidarSlam)


#
# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
set_property(TARGET slam PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")


#******************************************************************************
# Installer
#******************************************************************************

install(TARGETS LidarSlam
        RUNTIME DESTINATION ${SLAM_INSTALL_LIBRARY_DIR}
        LIBRARY DESTINATION ${SLAM_INSTALL_LIBRARY_DIR}
        ARCHIVE DESTINATION ${SLAM_INSTALL_LIBRARY_DIR}
        PUBLIC_HEADER DESTINATION ${SLAM_INSTALL_INCLUDE_DIR}/LidarSlam
        COMPONENT Runtime)


