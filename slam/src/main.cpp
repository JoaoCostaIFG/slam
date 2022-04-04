#include <bitset>
#include <iostream>
#include <fstream>
#include <chrono>
#include <random>

#include "../include/octomap/Octomap.h"
#include "../include/sonar/Scan.h"
#include "../include/sonar/Sonar.h"
#include "../include/sonar/Filters.h"

#include <opencv2/opencv.hpp>

using namespace std;
using namespace octomap;
using namespace sonar;

// https://segeval.cs.princeton.edu/public/off_format.html
vector<Vector3f> importOff(const string& filename) {
  ifstream f(filename);

  f.ignore(9999, '\n'); // first line should say OFF
  int vertCnt, faceCnt, edgeCnt; // edge cnt is usually ignored
  f >> vertCnt >> faceCnt >> edgeCnt;

  vector<Vector3f> ret;
  float x, y, z;
  for (int i = 0; i < vertCnt; ++i) {
    f >> x >> y >> z;
    ret.emplace_back(x, y, z);
  }

  // we don't care about the object faces
  f.close();
  return ret;
}

int main() {
  using chrono::high_resolution_clock;
  using chrono::milliseconds;
  using chrono::duration_cast;

  /*
  for (int i = 0; i < 5; ++i) {
    Octomap o = Octomap<>();

    auto startTime = high_resolution_clock::now();
    for (int x = -100; x < 100; x++) {
      for (int y = -100; y < 100; y++) {
        for (int z = -100; z < 100; z++) {
          Vector3f endpoint((float) x * 0.05f, (float) y * 0.05f, (float) z * 0.05f);
          o.setFull(endpoint, true); // integrate 'occupied' measurement
        }
      }
    }
    auto millis = duration_cast<milliseconds>(high_resolution_clock::now() - startTime).count();
    cout << "Ms: " << millis << " Secs: " << (double) millis / 1000.0 << endl;
    o.fix();
  }
   */

  //for (int i = -1; i < 2; i += 2) {
  //  for (int j = -1; j < 2; j += 2) {
  //    for (int k = -1; k < 2; k += 2) {
  //      auto endpointCoord = Vector3f(i, j, k);
  //      for (auto& point: o.rayCastBresenham(Vector3(), endpointCoord)) {
  //        o.setEmpty(point);
  //        o.setFull(endpointCoord);
  //      }
  //    }
  //  }
  //}

  //o.rayCastUpdate(Vector3(), Vector3f(1, 1, 1), 1.0);
  //o.rayCast(Vector3(), Vector3f(1267.09998, 2835.5, 272.221985));
  //for (auto &point: o.rayCast(Vector3(), Vector3f(67.09998, 35.5, 2.221985))) {
  //    o.setFull(*point);
  //}
  //o.setEmpty(Vector3f(67.09998, 35.5, 2.221985));

  /*
  std::ofstream file("bench.txt", std::ios_base::trunc);

  std::default_random_engine generator(std::hash<std::string>()("peedors"));
  float a = 10000.0, b = 2.0;
  std::normal_distribution<float> distribution(a, b);
  for (int cnt = 1000; cnt < 2000000; cnt *= 1.5) {
    Vector3f orig;
    Octomap o = Octomap<>();
    file << "Inserting to normal distribution of coordinates (" << a << ", " << b << ") " << cnt
         << " times\n";
    for (int i = 0; i < 5; ++i) {
      auto startTime = high_resolution_clock::now();
      for (int j = 0; j < cnt; ++j) {
        Vector3f dest(Vector3f(distribution(generator), distribution(generator), distribution(generator)));
        auto n = o.updateOccupancy(dest, 0.8);
      }
      auto millis = duration_cast<milliseconds>(high_resolution_clock::now() - startTime).count();
      file << "Ms: " << millis << " Secs: " << (double) millis / 1000.0 << endl;
    }
  }
   */

  //for (int i = 0; i < 1; ++i) {
  //  auto startTime = high_resolution_clock::now();
  //  o.pointcloudUpdate(importOff("../datasets/airplane_smaller.off"), Vector3f());
  //  auto millis = duration_cast<milliseconds>(high_resolution_clock::now() - startTime).count();
  //  cout << "Ms: " << millis << " Secs: " << (double) millis / 1000.0 << endl;
  //}

  // Reads data from json, displays cartesian and exports to octovis format
  //cout << "Size: " << o.getSize() << endl;
  //o.writeBinary("rust.bt");

  ifstream ss("../data.json");
  Scan* s = Scan::importJson(ss);

  Sweep* sweep = s->getSweeps().at(1);
  applyGaussian(*sweep, 9, 5);
  //displaySweep(*sweep, true);

  Sonar sonar;
  sonar.update(*sweep);

  return EXIT_SUCCESS;
}
