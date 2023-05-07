#include "../include/localization/Localization.h"

#include <cmath>

namespace localization {

template<typename T>
Localization<T>::Localization(const octomap::Vector3<>& initialPos,
                              const unsigned int particleCount,
                              const double displacementStddev, double distStddev,
                              const unsigned long seed)
    : generator(seed), displacementDistrib(0.0, displacementStddev),
      distDistrib(0.0, distStddev) {
  assert(particleCount != 0);

  this->particles.reserve(particleCount);
  for (unsigned i = 0; i < particleCount; ++i) {
    this->particles[i] = Particle(initialPos, 0);
  }
}

template<typename T>
void Localization<T>::update(const octomap::Octomap<T>& octomap,
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

template<typename T>
octomap::Vector3<> Localization<T>::generateNoise() {
  return {
      this->displacementDistrib(this->generator),
      this->displacementDistrib(this->generator),
      this->displacementDistrib(this->generator)
  };
}

template<typename T>
void Localization<T>::calcWeight(const octomap::Octomap<T>& octomap, Particle& particle,
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

template<typename T>
double Localization<T>::normalPDF(std::normal_distribution<double>& distrib, double x) {
  return pow(M_E, -pow(x - distrib.mean(), 2) / (2 * pow(distrib.stddev(), 2))) /
         (distrib.stddev() * sqrt(2 * M_PI));
}

}
