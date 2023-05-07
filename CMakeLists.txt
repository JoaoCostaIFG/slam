cmake_minimum_required(VERSION 3.16)
project(SLAM)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-Wall -pedantic -march=native -O2")

add_executable(SLAM slam/src/main.cpp slam/include/octomap/Octomap.h slam/include/octomap/OcNode.h slam/include/octomap/Vector3.h slam/include/octomap/OcNodeKey.h slam/include/sonar/Scan.h slam/src/sonar/Scan.cpp slam/include/octomap/OctomapIterator.h slam/include/sonar/Filters.h slam/include/sonar/Sonar.h slam/src/sonar/Sonar.cpp slam/include/hash_table/HashTable.h slam/include/hash_table/TableEntry.h slam/include/hash_table/HashTableIterator.h slam/include/hash_table/strategies/HashStrategy.h slam/include/hash_table/strategies/LinearHashStrategy.h slam/include/hash_table/strategies/QuadraticHashStrategy.h slam/include/hash_table/strategies/DoubleHashingStrategy.h slam/include/localization/Localization.h slam/include/localization/Particle.h slam/include/localization/Observation.h slam/include/localization/NeighborNode.h slam/include/sonar/Beam.h slam/include/sonar/Sweep.h slam/src/sonar/Beam.cpp slam/src/sonar/Sweep.cpp)

find_package(OpenCV REQUIRED)
find_package(RapidJSON REQUIRED)

find_package(OpenMP)
if (OpenMP_CXX_FOUND)
    target_link_libraries(SLAM PUBLIC OpenMP::OpenMP_CXX)
endif ()

set(Easy3D_DIR /usr/local/easy3d-2.5.2/lib/CMake/Easy3D/)
find_package(Easy3D COMPONENTS viewer REQUIRED)
target_link_libraries(SLAM PUBLIC ${OpenCV_LIBS} ${RapidJSON_LIBS} easy3d::viewer)

#target_link_libraries(SLAM PUBLIC ${OpenCV_LIBS} ${RapidJSON_LIBS})
