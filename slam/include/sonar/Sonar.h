#ifndef SLAM_SONAR_H
#define SLAM_SONAR_H

#include "../octomap/Vector3.h"
#include "../octomap/Octomap.h"
#include "../sonar/Scan.h"

using namespace octomap;

/** Speed (m/s) of sound underwater at depth 0m **/
#define UNDERWATER_VEL 1517
/** Tritech Micron SONAR values - sonar used for the project **/
/** Frequency (Hz) of the CRAS sub's sonar **/
#define SONAR_FREQ 700000
/** Beam spread (in rads) **/
#define Y_HORIZONTAL (3 / CV_PI)
#define Y_VERTICAL (35 / CV_PI)

namespace sonar {
  /** This class represents the sonar and its environment **/
  class Sonar {
  private:
    /** The position of the sonar **/
    Vector3<> position;
    /** Frequency between each sonar measurement **/
    double frequency;
    /** Beam horizontal spread **/
    double y_horiz;
    /** Beam vertical spread **/
    double y_vert;
    /** The map of the environment built from sonar measurements **/
    Octomap<> octomap;
  public:
    Sonar() : position(0.0, 0.0, 0.0), frequency(SONAR_FREQ), y_horiz(Y_HORIZONTAL), y_vert(Y_VERTICAL) {}

    /**
     * Updates the sonar map with the given data
     * @param sweep The sweep that holds the measurement data that will be used to update the map
     */
    void update(const Sweep& sweep);

    bool writeBinary(const std::string& outfilename) {
      return this->octomap.writeBinary(outfilename);
    }

    [[nodiscard]] const Vector3<>& getPosition() const { return position; }

    Octomap<>& getOctomap() {
      return octomap;
    }
  };
}

#endif //SLAM_SONAR_H
