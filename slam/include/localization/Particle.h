#ifndef SLAM_PARTICLE_H
#define SLAM_PARTICLE_H

#include "../octomap/Vector3.h"

namespace localization {

class Particle {
public:
  octomap::Vector3<> position;
  double weight;

  Particle(const octomap::Vector3<>& position, double weight) :
      position(position), weight(weight) {
  }
};

}


#endif //SLAM_PARTICLE_H
