#include <bitset>
#include <iostream>
#include <fstream>
#include <chrono>
#include <random>

#include "../include/octomap/Octomap.h"
#include "../include/HashTable/HashTable.h"
#include "../include/HashTable/HashTableIterator.h"
#include "../include/sonar/Scan.h"
#include "../include/sonar/Sonar.h"
#include "../include/sonar/Filters.h"

using namespace std;
using namespace octomap;
using namespace sonar;

using chrono::high_resolution_clock;
using chrono::milliseconds;
using chrono::duration_cast;

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
        o.updateOccupancy(dest, 0.8);
      }
      auto millis = duration_cast<milliseconds>(high_resolution_clock::now() - startTime).count();
      file << "Ms: " << millis << " Secs: " << (double) millis / 1000.0 << endl;
    }
  }
}

void menu() {
  std::cout << "Welcome to SLAM." << endl << endl;

  int option, finished = 0;
  while (finished == 0) {
    Octomap o = Octomap<>();
    std::cout << "What cloud point would you like to use?" << endl <<
              "\t1) Plane point cloud." << endl <<
              "\t2) AUV's collected point cloud." << endl <<
              "\t3) Other point cloud (should be found inside the folder \"datasets\")" << endl <<
              "\t4) Exit." << std::endl;
    std::cin >> option;
    switch (option) {
      case (1): {
        auto startTime = high_resolution_clock::now();
        o.pointcloudUpdate(importOff("../datasets/airplane_smaller.off"), Vector3f(), 1);
        auto millis = duration_cast<milliseconds>(high_resolution_clock::now() - startTime).count();
        cout << "Ms: " << millis << " Secs: " << (double) millis / 1000.0 << endl;
        o.writeBinary("plane.bt");
        cout << "\nResult saved as plane.bt\n\n";
        break;
      }
      case (2): {

        // Reads data from json, displays cartesian and exports to octovis format
        ifstream ss("../data.json");
        Scan* s = Scan::importJson(ss);
        Sonar sonar;

        auto sweeps = s->getSweeps();
        for (size_t i = 0; i < sweeps.size(); ++i) {
          Sweep* sweep = sweeps.at(i);
          // applyGaussian(*sweep, 9, 5);
          applyMedian(*sweep, 3);
          // displaySweep(*sweep, false);

          cout << "Doing sweep: " << i << endl;
          sonar.update(*sweep);
          sonar.writeBinary("auv-" + std::to_string(i) + ".bt");
        }

        break;
      }
      case (3): {
        string filename;
        std::cout << "What's the name of the .off file containing the desired points cloud (without .off)?"
                  << std::endl;
        std::cin >> filename;
        o.pointcloudUpdate(importOff("../datasets/" + filename + ".off"), Vector3f(), 1);
        o.writeBinary(filename + ".bt");
        cout << "\nResult saved as " << filename << ".bt\n\n";
        break;
      }
      case (4): {
        finished = 1;
        break;
      }
      default: {
        std::cout << "Wrong input, please try again." << std::endl;
        std::cin.clear();
        std::cin.ignore(256, '\n');
        break;
      }
    }
  }
}

int main() {
  menu();
  return EXIT_SUCCESS;
}