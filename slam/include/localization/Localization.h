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
               unsigned long seed = randomDevice()) : generator(seed),
                                                      displacementDistrib(0.0,
                                                                          displacementStddev),
                                                      distDistrib(0.0, distStddev) {
    assert(particleCount != 0);

    this->particles.reserve(particleCount);
    for (unsigned i = 0; i < particleCount; ++i) {
      this->particles[i] = Particle(initialPos, 0);
    }
  }

  void update(const octomap::Octomap<T>& octomap,
              const octomap::Vector3<>& displacement,
              const std::vector<Observation>& observations) {
    // TODO dynamic particle population size
    // TODO reuse the same vector instead of creating new
    // TODO check ressample offsets (there should be a minimum size and stuff)
    // TODO observation sample size should be a constant/configurable
    std::vector<Particle> newParticles;

    // incorporate odometry information
    newParticles.reserve(this->particles.size());
    for (size_t i = 0; i < this->particles.size(); ++i) {
      const octomap::Vector3<> noise = this->generateNoise();
      newParticles[i] = Particle(this->particles[i].position * displacement + noise,
                                 0.0);
    }

    // select random sample from observations
    const size_t sampleSize = 50;
    std::vector<Observation> obsSample;
    obsSample.reserve(sampleSize);
    std::sample(observations.begin(), observations.end(), std::back_inserter(obsSample),
                sampleSize, std::mt19937{randomDevice()});

    // calculate new particles weights
    for (auto& particle: newParticles) {
      this->calcWeight(octomap, particle, obsSample);
      particle.weight /= (double) newParticles.size();
    }

    // resample particles
    std::sort(newParticles.begin(), newParticles.end(),
              [](const Particle& a, const Particle& b) -> bool {
                return a.weight > b.weight;
              });
    long ressampleQuantity = (long) ceil((double) newParticles.size() / 2) - 1;
    newParticles.erase(newParticles.begin() + ressampleQuantity, newParticles.end());
    std::sample(newParticles.begin(), newParticles.end(),
                std::back_inserter(newParticles),
                ressampleQuantity, std::mt19937{randomDevice()});

    this->particles = newParticles;
  }

private:
  inline static std::random_device randomDevice;

  std::default_random_engine generator;
  std::normal_distribution<double> displacementDistrib, distDistrib;

  octomap::Vector3<> generateNoise() {
    return {
        this->displacementDistrib(this->generator),
        this->displacementDistrib(this->generator),
        this->displacementDistrib(this->generator)
    };
  }

  void calcWeight(const octomap::Octomap<T>& octomap, Particle& particle,
                  const std::vector<Observation>& observations) {
    for (auto& obs: observations) {
      // TODO R is constant 1 meter
      auto neighbors = this->searchNeighbors(octomap, obs.position - particle.position,
                                             1);
      for (auto& neighbor: neighbors) {
        particle.weight += this->normalPDF(this->distDistrib,
                                           particle.position.distanceTo(
                                               neighbor.position));
      }
    }
  }

  double normalPDF(std::normal_distribution<double>& distrib, double x) {
    return pow(M_E, -pow(x - distrib.mean(), 2) / (2 * pow(distrib.stddev(), 2))) /
           (distrib.stddev() * sqrt(2 * M_PI));
  }

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
