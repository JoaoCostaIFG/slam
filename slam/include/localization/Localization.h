#ifndef SLAM_LOCALIZATION_H
#define SLAM_LOCALIZATION_H

#include <algorithm>
#include <random>
#include <vector>
#include <unordered_set>

#include "Particle.h"
#include "Observation.h"
#include "NeighborNode.h"
#include "../octomap/Octomap.h"

namespace localization {

using namespace octomap;

template<typename T = uint16_t>
class Localization {
public:
  std::vector<Particle> particles;

  Localization(const octomap::Vector3<>& initialPos, unsigned particleCount,
               double displacementStddev, double distStddev,
               unsigned long seed = randomDevice());

  void update(const octomap::Octomap<T>& octomap,
              const octomap::Vector3<>& displacement,
              const std::vector<Observation>& observations);

private:
  inline static std::random_device randomDevice;

  std::default_random_engine generator;
  std::normal_distribution<double> displacementDistrib, distDistrib;

  octomap::Vector3<> generateNoise();

  void calcWeight(const octomap::Octomap<T>& octomap, Particle& particle,
                  const std::vector<Observation>& observations);

  double normalPDF(std::normal_distribution<double>& distrib, double x);

  /**
   * Returns a set of the nodes that are within the sphere created by the
   * center and radius parameters.
   * Note that the nodes might not be at the lowest level possible if the node
   * isn't divided. This means that the parent can be used in place of the child
   * (same information).
   * @param center
   * @param radius
   * @return
   */
  std::unordered_set<NeighborNode<T>>
  searchNeighbors(const Octomap<T> octomap, const Vector3<>& center,
                  const double radius) {
    std::unordered_set<NeighborNode<T>> ret;
    for (double x = center.x() - radius;
         x < center.x() + radius; x += this->resolution) {
      for (double y = center.y() - radius;
           y < center.y() + radius; y += this->resolution) {
        for (double z = center.z() - radius;
             z < center.z() + radius; z += this->resolution) {
          double xDiff = x - center.x();
          double yDiff = y - center.y();
          double zDiff = z - center.z();
          if (xDiff * xDiff + yDiff * yDiff + zDiff * zDiff < radius * radius) {
            // inside sphere created by the radius
            OcNodeKey<T> key({x, y, z});
            auto n = octomap->search(key);
            ret.insert(NeighborNode(OcNodeKey<T>::key2coord(key), n));
          }
        }
      }
    }
    return ret;
  }
};

}


#endif //SLAM_LOCALIZATION_H
