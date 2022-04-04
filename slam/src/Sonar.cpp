
#include "../include/sonar/Sonar.h"


namespace sonar {

  void Sonar::update(const Sweep& sweep) {
    for (const Beam* beam: sweep.getBeams()) {
      size_t obstacle_index = beam->getObstacleST();

      // TODO Use sonar position instead of center of axis
      Vector3<> obstacle = beam->atVec(obstacle_index);
      float prob = (float) beam->at(obstacle_index) / MAX_INTENSITY;
      this->octomap.rayCastUpdate(this->position, obstacle, 1); // TODO Don't use prob of 1
    }

    this->octomap.writeBinary("test.bt");
  }
}