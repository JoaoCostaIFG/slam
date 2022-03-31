#include <bitset>
#include <iostream>
#include <fstream>
#include <ctime>

#include "../include/octomap/Octomap.h"
#include "../include/Scan.h"

#include <opencv2/opencv.hpp>

using namespace std;
using namespace octomap;

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
//    clock_t start = clock();
//    o.pointcloudUpdate(importOff("../datasets/airplane_smaller.off"), Vector3f());
//    //o.discretizedPointcloudUpdate(importOff("../datasets/airplane_smaller.off"), Vector3f());
//    cout << float(clock() - start) / CLOCKS_PER_SEC << endl;
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
  s->getSweeps().at(22)->display(true);
  s->getSweeps().at(1)->display(true);

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
