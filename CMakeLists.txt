cmake_minimum_required(VERSION 3.21)
project(SLAM)

set(CMAKE_CXX_STANDARD 20)

add_executable(SLAM slam/src/main.cpp slam/src/Octomap.cpp slam/include/octomap/Octomap.h slam/src/Ocnode.cpp slam/include/octomap/Ocnode.h slam/include/octomap/Vector3.h)