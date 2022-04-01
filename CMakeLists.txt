cmake_minimum_required(VERSION 3.16)
project(SLAM)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-Wall -pedantic")

add_executable(SLAM slam/src/main.cpp slam/include/octomap/Octomap.h slam/include/octomap/OcNode.h slam/include/octomap/Vector3.h slam/include/octomap/OcNodeKey.h slam/include/sonar/Scan.h slam/src/Scan.cpp slam/include/octomap/OctomapIterator.h slam/include/sonar/Filters.h slam/include/sonar/Sonar.h slam/src/Sonar.cpp)

find_package(Boost REQUIRED)
find_package(OpenCV REQUIRED)
find_package(RapidJSON REQUIRED)
find_package(OpenMP)

if (OpenMP_CXX_FOUND)
    target_link_libraries(SLAM PUBLIC OpenMP::OpenMP_CXX ${OpenCV_LIBS} ${RapidJSON_LIBS})
endif ()
