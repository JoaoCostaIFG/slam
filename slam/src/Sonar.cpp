
#include "../include/sonar/Sonar.h"


namespace sonar {

  void Sonar::update(const Sweep& sweep) {
    for (const Beam* beam: sweep.getBeams()) {
      size_t obstacle_index = beam->getObstacleST();

      double base_angle = beam->getAngle();
      std::vector<Vector3<>> pointCloud;

      for (double angle = base_angle - 1.5;
          angle < base_angle + 1.601; angle += 0.1) { // TODO Replace 1.601 with angle step
        double angle_rad = ((angle + 180) * CV_PI) / 180;
        Vector3<> v = beam->atVec(obstacle_index, angle_rad);
        int x = round(v.x()), y = round(v.y());
        pointCloud.emplace_back(x, y, 0);
      }

      // TODO Don't use prob of 1
      // float prob = (float) beam->at(obstacle_index) / MAX_INTENSITY;
      this->octomap.pointcloudUpdate(pointCloud, this->position, 1);
    }

    this->octomap.writeBinary("test.bt");
  }
}