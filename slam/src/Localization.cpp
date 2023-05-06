#include "../include/localization/Localization.h"

namespace localization {

Localization::Localization(const octomap::Vector3<>& initialPos,
                           const unsigned int particleCount,
                           const double displacementStddev, const unsigned long seed)
    : generator(seed), displacementDistrib(0.0, displacementStddev) {
  assert(particleCount != 0);

  this->particles.reserve(particleCount);
  for (unsigned i = 0; i < particleCount; ++i) {
    this->particles[i] = Particle(initialPos, 0);
  }
}

void Localization::update(const octomap::Vector3<>& displacement,
                          const std::vector<Observation>& observations) {
  std::vector<Particle> newParticles;

  // incorporate odometry information
  newParticles.reserve(this->particles.size());
  for (size_t i = 0; i < this->particles.size(); ++i) {
    const octomap::Vector3<> noise = octomap::Vector3<>(
        this->displacementDistrib(this->generator),
        this->displacementDistrib(this->generator),
        this->displacementDistrib(this->generator)
    );
    newParticles[i] = Particle(this->particles[i].position * displacement + noise,
                               1.0);
  }

  // select random sample from observations
  const size_t sampleSize = 50;
  std::vector<Observation> obsSample;
  obsSample.reserve(sampleSize);
  std::sample(observations.begin(), observations.end(), std::back_inserter(obsSample),
              sampleSize, std::mt19937{randomDevice()});

  // calculate new particles weights
  for (auto& particle: newParticles) {
    double weight = 0;
    for (auto& obs: obsSample) {
      weight += this->calcWeight();
    }
    weight *= 1 / (double) newParticles.size();
  }
}

}
