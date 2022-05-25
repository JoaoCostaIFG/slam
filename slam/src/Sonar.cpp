
#include "../include/sonar/Sonar.h"

#include <algorithm>
#include <cmath>


namespace sonar {

  void Sonar::update(const Sweep& sweep) {
    for (const Beam* beam: sweep.getBeams()) {
      size_t obstacle_index = beam->getObstacleST();

      double base_angle = beam->getAngle();
      std::vector<Vector3<>> pointCloud;

      for (double angle = base_angle - 1.5;
           angle < base_angle + 1.601; angle += 0.1) { // TODO Replace 1.601 with angle step
        double angle_rad = ((angle + 180) * CV_PI) / 180;
        Vector3f v = beam->coordToReal(beam->measurementToCartesian(obstacle_index, angle_rad));
        pointCloud.push_back(v);
      }

      // TODO Use sonar position instead of center of axis
      double prob = double(unsigned(beam->at(obstacle_index))) / 255.0;
      for (const auto& dest: pointCloud) {
        this->octomap.rayCastUpdate(this->position, dest, prob);
      }
      this->octomap.pointcloudUpdate(pointCloud, this->position, prob);
    }
  }
}