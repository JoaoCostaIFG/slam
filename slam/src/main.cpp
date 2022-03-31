#include <bitset>
#include <iostream>
#include <fstream>
#include <chrono>

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
//  using chrono::high_resolution_clock;
//  using chrono::milliseconds;
//  using chrono::duration_cast;
//
//  Octomap o = Octomap();
  //o.rayCastUpdate(Vector3(), Vector3f(1, 1, 1), 1.0);


  //for (int x = -100; x < 100; x++) {
  //  for (int y = -100; y < 100; y++) {
  //    for (int z = -100; z < 100; z++) {
  //      Vector3f endpoint((float) x * 0.05f, (float) y * 0.05f, (float) z * 0.05f);
  //      //cout << endpoint << endl;
  //      o.setFull(endpoint); // integrate 'occupied' measurement
  //    }
  //  }
  //}

  //for (int i = -1; i < 2; i += 2) {
  //  for (int j = -1; j < 2; j += 2) {
  //    for (int k = -1; k < 2; k += 2) {
  //      auto endpointCoord = Vector3f(i, j, k);
  //      for (auto& point: o.rayCastBresenham(Vector3(), endpointCoord)) {
  //        o.setEmpty(*point);
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

//  for (int i = 0; i < 3; ++i) {
//    auto startTime = high_resolution_clock::now();
//    o.pointcloudUpdate(importOff("../datasets/airplane_smaller.off"), Vector3f());
//    auto millis = duration_cast<milliseconds>(high_resolution_clock::now() - startTime).count();
//    cout << "Ms: " << millis << " Secs: " << (double) millis / 1000.0 << endl;
//  }

  //auto ray = o.rayCast(Vector3<float>(0, 0, 0), Vector3<float>(1.0, 1.0, 1.0));
  //for (auto & it : ray) {
  //    cout << it->toCoord() << endl;
  //    o.setOccupancy(*it, 1.0);
  //}
  //o.setFull(Vector3f(1.0, 1.0, 1.0));

//  cout << "Size: " << o.getSize() << endl;
//  o.writeBinary("rust.bt");

  ifstream ss("../data.json");
  Scan *s = Scan::importJson(ss);
  cout << *s << endl;

  // s->getSweeps().at(0)->display(false);
  Sweep *sweep = s->getSweeps().at(1);
  applyGaussian(*sweep, 9, 5);
  displaySweep(*sweep, true);

  Sonar sonar;
  sonar.update(*sweep);

//  uint8_t  v1[] = {1, 2, 3};
//  uint8_t  v2[] = {4, 255, 6};
//  uint8_t  v3[] = {7, 8, 9};
//  uint8_t *m[] = {v1, v2, v3};
//

//  std::string image_path = "../cartesian.png";
//  cv::Mat img = cv::Mat(s->getBeamNo(), s->getBeamLen(), CV_8U, s->getIntensities().data());
//
//  cv::namedWindow("Display Iage", cv::WINDOW_AUTOSIZE);
//  cv::imshow("Display window", img);
//
//  int k;
//  do {
//    k = cv::waitKey(0); // Wait for a keystroke in the window
//  } while (k != 'q');

  return EXIT_SUCCESS;
}
