#ifndef SLAM_FILTERS_H
#define SLAM_FILTERS_H

#include <opencv2/opencv.hpp>

#include "Scan.h"

using namespace sonar;

namespace sonar {
  void applyGaussian(Sweep& sweep, int kernelSize, int sigma) {
    cv::GaussianBlur(sweep.getIntensities(), sweep.getIntensities(), cv::Size(kernelSize, kernelSize), sigma);
  }

  void applyMean(Sweep& sweep, int kernelSize) {
    cv::InputArray arr(sweep.getIntensities());
    cv::blur(sweep.getIntensities(), sweep.getIntensities(), cv::Size(kernelSize, kernelSize));
  }

  void applyMedian(Sweep& sweep, int kernelSize) {
    cv::InputArray arr(sweep.getIntensities());
    cv::medianBlur(sweep.getIntensities(), sweep.getIntensities(), kernelSize);
  }

  /* Opens an openCV window with a sweep sonar (polar). Note: Waits for user to press q to close */
  void displaySweep(const Sweep& sweep, const bool& inCartesian = false) {
    cv::Mat img;
    if (inCartesian) {
      size_t n = sweep.getBeamLen() * 2;
      std::vector<int> vote_intensities(n * n, 0);
      std::vector<int> vote_count(n * n, 0);
      img = cv::Mat::zeros(n, n, CV_8U);

      // Gather votes
      int x0 = sweep.getBeamLen(), y0 = sweep.getBeamLen();
      for (const Beam* beam: sweep.getBeams()) {
        double base_angle = beam->getAngle();
        for (double angle = base_angle - 1.5;
             angle < base_angle + 1.601; angle += 0.1) { // TODO Replace 1.601 with angle step
          // 180 => pi
          // deg => rad
          for (size_t i = 0; i < sweep.getBeamLen(); ++i) {
            const uint8_t& intensity = beam->at(i);
            double angle_rad = ((angle + 180) * CV_PI) / 180;
            Vector3<> v = beam->atVec(i, angle_rad);
            int x = x0 + round(v.x()), y = y0 + round(v.y());
            int pos = y + x * n;
            vote_intensities[pos] += intensity;
            vote_count[pos] += 1;
          }
        }
      }

      for (size_t y = 0; y < n; y++) {
        for (size_t x = 0; x < n; x++) {
          int pos = y + x * n;
          int cnt = vote_count[pos];
          if (cnt != 0) {
            uint8_t mean = round(vote_intensities[pos] / (double) cnt);
            img.at<uchar>(y, x) = mean;
          }
        }
      }

    } else { // Polar Coordinates
      // img = cv::Mat(sweep.getBeams().size(), sweep.getBeamLen(), CV_8U, sweep.getIntensities().data());
      img = sweep.getIntensities();
    }

    cv::namedWindow("Sweep" + std::to_string(sweep.getSweepNo()), cv::WINDOW_AUTOSIZE);
    cv::imshow("Sweep" + std::to_string(sweep.getSweepNo()), img);

    int k;
    do {
      k = cv::waitKey(0); // Wait for a keystroke in the window
    } while (k != 'q');
  }

}

#endif //SLAM_FILTERS_H
