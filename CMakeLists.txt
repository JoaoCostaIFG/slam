cmake_minimum_required(VERSION 3.16)
project(SLAM)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-Wall -pedantic -march=native -O2")

add_executable(SLAM slam/src/main.cpp slam/include/octomap/Octomap.h slam/include/octomap/OcNode.h slam/include/octomap/Vector3.h slam/include/octomap/OcNodeKey.h slam/include/sonar/Scan.h slam/src/Scan.cpp slam/include/octomap/OctomapIterator.h slam/include/sonar/Filters.h slam/include/sonar/Sonar.h slam/src/Sonar.cpp slam/include/HashTable/HashTable.h slam/include/HashTable/TableEntry.h slam/include/HashTable/HashTableIterator.h)

find_package(OpenCV REQUIRED)
find_package(RapidJSON REQUIRED)
find_package(OpenMP)

if (OpenMP_CXX_FOUND)
    target_link_libraries(SLAM PUBLIC OpenMP::OpenMP_CXX)
endif ()

target_link_libraries(SLAM PUBLIC ${OpenCV_LIBS} ${RapidJSON_LIBS})
