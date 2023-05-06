#ifndef SLAM_OBSERVATION_H
#define SLAM_OBSERVATION_H

#include "../octomap/Vector3.h"

namespace localization {

class Observation {
public:
  octomap::Vector3<> position;
  double logOdds;

  Observation(const octomap::Vector3<>& position, double logOdds) :
      position(position), logOdds(logOdds) {
  }
};

}

#endif //SLAM_OBSERVATION_H
