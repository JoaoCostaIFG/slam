cmake_minimum_required(VERSION 3.21)
project(SLAM)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-Wall -pedantic")

find_package(Boost REQUIRED)
add_executable(SLAM slam/src/main.cpp slam/src/Octomap.cpp slam/include/octomap/Octomap.h slam/src/OcNode.cpp slam/include/octomap/OcNode.h slam/include/octomap/Vector3.h slam/src/OcNodeKey.cpp slam/include/octomap/OcNodeKey.h)
