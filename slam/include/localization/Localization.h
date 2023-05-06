#ifndef SLAM_LOCALIZATION_H
#define SLAM_LOCALIZATION_H

#include <algorithm>
#include <random>
#include <vector>

#include "Particle.h"
#include "Observation.h"

namespace localization {

class Localization {
private:
  std::default_random_engine generator;
  std::normal_distribution<double> displacementDistrib;
  static std::random_device randomDevice;

public:
  std::vector<Particle> particles;

  Localization(const octomap::Vector3<>& initialPos, unsigned particleCount,
               double displacementStddev, unsigned long seed = randomDevice());

  void update(const octomap::Vector3<>& displacement,
              const std::vector<Observation>& observations);

  double calcWeight() {

  }
};

}


#endif //SLAM_LOCALIZATION_H
