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

void benchmark() {
  using chrono::high_resolution_clock;
  using chrono::milliseconds;
  using chrono::duration_cast;

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
}

int main() {
  /*
  // insert some measurements of free cells
  for (float x = -2; x <= 0; x += 0.02f) {
    for (float y = -2; y <= 0; y += 0.02f) {
      for (float z = -2; z <= 0; z += 0.02f) {
        Vector3f endpoint(x, y, z);
        o.setEmpty(endpoint); // integrate 'free' measurement
      }
    }
  }
  // insert some measurements of occupied cells (twice as much)
  for (float x = -1; x <= 0; x += 0.01f) {
    for (float y = -1; y <= 0; y += 0.01f) {
      for (float z = -1; z <= 0; z += 0.01f) {
        Vector3f endpoint(x, y, z);
        o.setFull(endpoint); // integrate 'occupied' measurement
      }
    }
  }
  o.rayCastUpdate(Vector3f(0, 0, 0), Vector3f(-2, -2, -2), 0.5);
   */


  // Reads data from json, displays cartesian and exports to octovis format
  ifstream ss("../data.json");
  Scan* s = Scan::importJson(ss);

  Sweep* sweep = s->getSweeps().at(1);
  applyGaussian(*sweep, 9, 5);
  displaySweep(*sweep, true);

  Sonar sonar;
  sonar.update(*sweep);

  return EXIT_SUCCESS;
}
