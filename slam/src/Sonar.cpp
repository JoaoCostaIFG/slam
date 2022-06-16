
#include "../include/sonar/Sonar.h"

#include <algorithm>


namespace sonar {

  std::vector<Vector3<>> Sonar::getBeamEndpoints2D(const Beam* beam, size_t obstacle_index) const {
    std::vector<Vector3<>> pointCloud;
    // In 2D We only want to get the beam's center point across the obstacle
    Vector3<> centerPoint = beam->coordToReal(beam->getCenterPoint(obstacle_index));
    pointCloud.push_back(centerPoint);
    return pointCloud;
  }

  std::vector<Vector3<>> Sonar::getBeamEndpoints3D(const Beam* beam, size_t obstacle_index,
                                                   const unsigned& ndiv_horiz, const unsigned& ndiv_vert) const {
    assert(ndiv_horiz != 0);
    assert(ndiv_vert != 0);

    std::vector<Vector3<>> pointCloud;
    Vector3<> centerPoint = beam->coordToReal(beam->getCenterPoint(obstacle_index));
    pointCloud.push_back(centerPoint);
    // Horizontal Plane = xOy
    double step_xoy = this->y_horiz / ndiv_horiz, low_horiz = beam->getAngle() - (this->y_horiz / 2);
    // Vertical Plane = yOz
    double step_yoz = this->y_vert / ndiv_vert, low_vert = -this->y_vert / 2;

    double xoy_angle = low_horiz;
    for (size_t i = 0; i < ndiv_horiz + 1; ++i) {
      double xoy_rad = (xoy_angle * CV_PI) / 180;
      double yoz_angle = low_vert;
      for (size_t j = 0; j < ndiv_vert + 1; ++j) {
        double yoz_rad = (yoz_angle * CV_PI) / 180;

        double x = cos(xoy_rad) * (double) obstacle_index;
        double y = sin(xoy_rad) * (double) obstacle_index;
        double z = sin(yoz_rad) * (double) obstacle_index;
        Vector3<> point = Vector3<>(x, y, z);
        pointCloud.push_back(beam->coordToReal(point));
        yoz_angle += step_yoz;
      }
      xoy_angle += step_xoy;
    }


    return pointCloud;
  }

  void Sonar::update(const Sweep& sweep) {
    for (const Beam* beam: sweep.getBeams()) {
      size_t obstacle_index = beam->getObstacleST();
      std::vector<Vector3<>> pointCloud = this->getBeamEndpoints3D(beam, obstacle_index, 16, 16);

      // TODO Use sonar position instead of center of axis
      float prob = float(unsigned(beam->at(obstacle_index))) / 255.0;
      for (const auto& dest: pointCloud) {
        this->octomap.rayCastUpdate(this->position, dest, prob);
      }
      this->octomap.discretizedPointcloudUpdate(pointCloud, this->position, prob);
    }
  }
}