#ifndef SLAM_SONAR_H
#define SLAM_SONAR_H

#include "../octomap/Vector3.h"
#include "../octomap/Octomap.h"
#include "../sonar/Scan.h"

using namespace octomap;

namespace sonar {
  /** This class represents the sonar and its environment **/
  class Sonar {
  private:
    /** The position of the sonar **/
    Vector3<> position;
    /** The map of the environment built from sonar measurements **/
    Octomap<> octomap;
  public:
    Sonar() : position(0.0, 0.0, 0.0) {}

    /**
     * Updates the sonar map with the given data
     * @param sweep The sweep that holds the measurement data that will be used to update the map
     */
    void update(const Sweep& sweep);

    bool writeBinary(const std::string& outfilename) {
      return this->octomap.writeBinary(outfilename);
    }

    [[nodiscard]] const Vector3<>& getPosition() const { return position; }
  };
}

#endif //SLAM_SONAR_H
