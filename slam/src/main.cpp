#include <bitset>
#include <iostream>
#include <chrono>
#include <random>
#include <unordered_set>

#include "../include/octomap/Octomap.h"
#include "../include/sonar/Scan.h"
#include "../include/sonar/Sonar.h"
#include "../include/sonar/Filters.h"

using namespace std;
using namespace octomap;
using namespace sonar;

using chrono::high_resolution_clock;
using chrono::microseconds;
using chrono::duration_cast;

// https://segeval.cs.princeton.edu/public/off_format.html
vector<Vector3<>> importOff(const string& filename) {
  ifstream f(filename);

  f.ignore(9999, '\n'); // first line should say OFF
  int vertCnt, faceCnt, edgeCnt; // edge cnt is usually ignored
  f >> vertCnt >> faceCnt >> edgeCnt;

  vector<Vector3<>> ret;
  float x, y, z;
  for (int i = 0; i < vertCnt; ++i) {
    f >> x >> y >> z;
    ret.emplace_back(x, y, z);
  }

  // we don't care about the object faces
  f.close();
  return ret;
}

void benchmarkInsert() {
  std::ofstream file("benchmark_set_insert_nodups_quad_inplace.txt",
                     std::ios_base::trunc);

  std::default_random_engine generator(std::hash<std::string>()("peedors"));
  float a = 10000.0, b = 5.0;
  std::normal_distribution<float> distribution(a, b);

  file
      << "Number of inserts: (microseconds, number of duplicates, number of colisions)x5\n";

  for (unsigned int cnt = 1000; cnt <= 4000000; cnt += (cnt / 10)) {
    //cout << cnt << "\n";
    file << cnt << ":";
    for (int i = 0; i < 5; ++i) {
      HashTable::HashTable<Vector3f> h(32,
                                       new HashTable::QuadraticHashStrategy<Vector3f>());
      auto startTime = high_resolution_clock::now();
      unsigned int dups = 0;
      for (unsigned int j = 0; j < cnt; ++j) {
        //auto v = Vector3f(distribution(generator));
        auto v = Vector3f(distribution(generator), distribution(generator),
                          distribution(generator));
        if (!h.insert(v))
          ++dups;
      }
      auto micros = duration_cast<microseconds>(
          high_resolution_clock::now() - startTime).count();
      file << " (" << micros << ", " << dups << ", " << h.collisions << ")";
    }
    file << "\n";
  }
}

void benchmarkLookup() {
  unsigned int lookupCnt = 600000;
  std::ofstream file("benchmark_set_lookup_existing_600000.txt", std::ios_base::trunc);

  std::default_random_engine generator(std::hash<std::string>()("peedors"));
  float a = 10000.0, b = 5.0;
  std::normal_distribution<float> distribution(a, b);

  unsigned int cnt = lookupCnt * 8;
  HashTable::HashTable<Vector3f> h(cnt * 2,
                                   new HashTable::QuadraticHashStrategy<Vector3f>());

  file
      << "Perform lookups that don't exist in set. Number of lookups: time. Number of inserts: "
      << cnt << "\n";

  for (unsigned int i = 0; i < cnt; ++i) {
    auto v = Vector3f(distribution(generator), distribution(generator),
                      distribution(generator));
    h.insert(v);
  }

  cout << "Insertei\n";

  auto it = h.begin();
  //for (unsigned int i = 0; i < cnt / 4; ++i) ++it;
  //auto lookup = it->getValue();
  //h.remove(lookup);

  for (int i = 0; i < 5; ++i) {
    auto startTime = high_resolution_clock::now();
    for (unsigned int j = 0; j < lookupCnt; ++j) {
      auto lookup = it->getValue();
      if (!h.contains(lookup)) {
        [[unlikely]]
            cout << "Something went wrong, element not found.\n";
      }
      ++it;
    }
    auto micros = duration_cast<microseconds>(
        high_resolution_clock::now() - startTime).count();
    file << lookupCnt << ": " << micros << "\n";
  }
}

void benchmarkMerge() {
  std::ofstream file("benchmark_set_merge_resize.txt", std::ios_base::trunc);

  std::default_random_engine generator(std::hash<std::string>()("peedors"));
  float a = 10000.0, b = 5.0;
  std::normal_distribution<float> distribution(a, b);

  file << "Merge 2 big sets. Number of inserts on each: times\n";

  for (unsigned int cnt = 250000; cnt <= 2500000; cnt += cnt / 10) {
    cout << cnt << "\n";

    file << cnt << ":";
    for (int i = 0; i < 5; ++i) {
      HashTable::HashTable<Vector3f> h(cnt * 2,
                                       new HashTable::QuadraticHashStrategy<Vector3f>());
      HashTable::HashTable<Vector3f> h2(cnt * 2,
                                        new HashTable::QuadraticHashStrategy<Vector3f>());
      for (unsigned int j = 0; j < cnt; ++j) {
        auto v = Vector3f(distribution(generator), distribution(generator),
                          distribution(generator));
        h.insert(v);
        //if (((int) distribution(generator)) % 2 == 0)
        //  h2.insert(v);
        //else
        //  h2.insert(Vector3f(distribution(generator), distribution(generator), distribution(generator)));
        h2.insert(Vector3f(distribution(generator), distribution(generator),
                           distribution(generator)));
      }

      auto startTime = high_resolution_clock::now();
      h.merge(h2, true);
      auto micros = duration_cast<microseconds>(
          high_resolution_clock::now() - startTime).count();
      file << " " << micros;
    }
    file << "\n";
  }
}

void benchmarkcppset() {
  std::ofstream file("benchmark_cppset_merge.txt", std::ios_base::trunc);

  std::default_random_engine generator(std::hash<std::string>()("peedors"));
  float a = 10000.0, b = 5.0;
  std::normal_distribution<float> distribution(a, b);

  file << "Merge 2 big sets. Number of inserts on each: times\n";

  for (unsigned int cnt = 250000; cnt <= 2500000; cnt += cnt / 10) {
    cout << cnt << "\n";

    file << cnt << ":";
    for (int i = 0; i < 5; ++i) {
      std::unordered_set<Vector3f, Vector3f::Hash, Vector3f::Cmp> h;
      std::unordered_set<Vector3f, Vector3f::Hash, Vector3f::Cmp> h2;
      for (unsigned int j = 0; j < cnt; ++j) {
        auto v = Vector3f(distribution(generator), distribution(generator),
                          distribution(generator));
        h.insert(v);
        //if (((int) distribution(generator)) % 2 == 0)
        //  h2.insert(v);
        //else
        //  h2.insert(Vector3f(distribution(generator), distribution(generator), distribution(generator)));
        h2.insert(Vector3f(distribution(generator), distribution(generator),
                           distribution(generator)));
      }

      auto startTime = high_resolution_clock::now();
      h.insert(h2.begin(), h2.end());
      auto micros = duration_cast<microseconds>(
          high_resolution_clock::now() - startTime).count();
      file << " " << micros;
    }
    file << "\n";
  }
}

void benchmark() {
  std::ofstream file("real_test.txt", std::ios_base::trunc);

  file << "Real test\n";

  for (int i = 0; i < 5; ++i) {
    Octomap o = Octomap<>();

    auto startTime = high_resolution_clock::now();
    o.pointcloudUpdate(importOff("../datasets/airplane_smaller.off"), Vector3<>(), 1);
    auto micros = duration_cast<microseconds>(
        high_resolution_clock::now() - startTime).count();
    file << micros << "\n";

    cout << (micros / 1000) / 1000.0 << "\n";
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
              "\t3) Other point cloud (should be found inside the folder \"datasets\")"
              << endl <<
              "\t4) Exit." << std::endl;
    std::cin >> option;
    switch (option) {
      case (1): {
        o.pointcloudUpdate(importOff("../datasets/airplane_smaller.off"), Vector3<>(),
                           1);
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
        std::cout
            << "What's the name of the .off file containing the desired points cloud (without .off)?"
            << std::endl;
        std::cin >> filename;
        o.pointcloudUpdate(importOff("../datasets/" + filename + ".off"), Vector3<>(),
                           1);
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
  //benchmark();

  return EXIT_SUCCESS;
}