#ifndef SLAM_SONAR_H
#define SLAM_SONAR_H

#include "../octomap/Vector3.h"
#include "../octomap/Octomap.h"
#include "../sonar/Scan.h"

using namespace octomap;

namespace sonar {
  class Sonar {
  private:
    Vector3<> position;
    Octomap octomap;
  public:
    Sonar() : position(0.0, 0.0, 0.0) {}

    void update(const Sweep &sweep);

    [[nodiscard]] const Vector3<>& getPosition() const { return position; }
  };
}

#endif //SLAM_SONAR_H
