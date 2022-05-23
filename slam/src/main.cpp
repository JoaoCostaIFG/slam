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
        o.pointcloudUpdate(importOff("../datasets/airplane_0119.off"), Vector3f(), 1);
        o.writeBinary("plane.bt");
        cout << "\nResult saved as plane.bt\n\n";
        break;
      }
      case (2): {
        ifstream ss("../data.json");
        Scan* s = Scan::importJson(ss);

        Sweep* sweep = s->getSweeps().at(1);
        applyGaussian(*sweep, 9, 5);

        Sonar sonar;
        sonar.update(*sweep);
        cout << "\nResult saved as auv.bt\n\n";
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
  //menu();

//  auto h1 = HashTable::HashTable<Vector3<int>>(10);
//  auto h2 = HashTable::HashTable<Vector3<int>>(10);
//  auto v = Vector3<int>(2,1,4);
//  auto v1 = Vector3<int>(2,2,4);
//  auto v2 = Vector3<int>(2,3,4);
//  auto v3 = Vector3<int>(2,4,4);
//  auto v4 = Vector3<int>(2,5,4);
//  auto v5 = Vector3<int>(2,6,4);
//  auto v6 = Vector3<int>(2,7,4);
//  auto v7 = Vector3<int>(2,8,4);
//  auto v8 = Vector3<int>(2,9,4);
//  auto v9 = Vector3<int>(2,0,4);
//  auto v10 = Vector3<int>(2,10,4);
//  vector<Vector3<int>*> vec;
//  vec.push_back(&v1);
//  vec.push_back(&v2);
//  vec.push_back(&v3);
//
//  auto hti = HashTableIterator::HashTableIterator<Vector3<int>*>(vec.begin(), vec.end());
//
//  cout << **hti << endl;
//  hti++;
//  cout << **hti << endl;
//  hti++;
//  cout << **hti << endl;
//  hti++;
//  cout << **hti << endl;
//  hti++;
//  cout << (hti!=hti) << endl;

//
//
//  h1.insert(vec.begin(), vec.end());
//  h1.printAll();
//  std::cout << h1.contains(v) << std::endl;
//  h1.insert(v1);
//  h1.insert(v2);
//  h1.insert(v3);
//  h1.insert(v4);
//  h1.insert(v5);
//  h1.insert(v6);
//  h1.printAll();
//
//  h2.insert(v7);
//  h2.insert(v8);
//  h2.insert(v9);
//  h2.insert(v10);
//
//  h2.printAll();
//  h1.merge(h2);
//  std::cout << "----------------------------------------" << std::endl;
//  h1.printAll();
//  std::cout << "----------------------------------------" << std::endl;
//  h2.printAll();

  return EXIT_SUCCESS;
}