
#include "../include/sonar/Sonar.h"

#include <algorithm>


namespace sonar {

  void Sonar::update(const Sweep& sweep) {
    size_t max_beam = 0;
    for (const Beam* beam: sweep.getBeams()) {
      size_t obstacle_index = beam->getObstacleST();

      size_t max_prob = 0;
      for (size_t i=0; i < 399; ++i) {
        if (beam->getIntensities()[i] > max_prob)
          max_prob = beam->getIntensities()[i];
      }

      if (max_prob > max_beam )
        max_beam = max_prob;

      double base_angle = beam->getAngle();
      std::vector<Vector3<>> pointCloud;

      for (double angle = base_angle - 1.5;
           angle < base_angle + 1.601; angle += 0.1) { // TODO Replace 1.601 with angle step
        double angle_rad = ((angle + 180) * CV_PI) / 180;
        Vector3f v = beam->coordToReal(beam->measurementToCartesian(obstacle_index, angle_rad));
        pointCloud.push_back(v);
      }

      // TODO Use sonar position instead of center of axis
      float prob = (float) beam->at(obstacle_index) / MAX_INTENSITY;
      // std::cout << "PROB " << prob << std::endl;
      for (const auto& dest: pointCloud) {
        this->octomap.rayCastUpdate(this->position, dest, prob);
      }
      this->octomap.pointcloudUpdate(pointCloud, this->position, prob);
    }
  }
}