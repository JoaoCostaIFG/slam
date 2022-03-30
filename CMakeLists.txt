cmake_minimum_required(VERSION 3.16)
project(SLAM)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-Wall -pedantic")

add_executable(SLAM slam/src/main.cpp slam/src/Octomap.cpp slam/include/octomap/Octomap.h slam/src/OcNode.cpp slam/include/octomap/OcNode.h slam/include/octomap/Vector3.h slam/include/octomap/OcNodeKey.hxx slam/include/Scan.h slam/src/Scan.cpp slam/include/octomap/OctomapIterator.h)

find_package(Boost REQUIRED serialization)

find_package(OpenMP)
if (OpenMP_CXX_FOUND)
    target_link_libraries(SLAM PUBLIC OpenMP::OpenMP_CXX ${Boost_LIBRARIES})
endif ()
