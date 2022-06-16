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
#define Y_HORIZONTAL 3
#define Y_VERTICAL 35

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


    /**
     * Retrieves a list of estimated points, in 2D, that hit an obstacle in the given obstacle_index
     * across a given beam (in its angle)
     * @param beam Beam whose intersection will be estimated
     * @param obstacle_index Index of the measurement that hit the obstacle
     * @return Vector of points that belong to the beam that have hit the obstacle
     */
    std::vector<Vector3<>> getBeamEndpoints2D(const Beam* beam, size_t obstacle_index) const;


    /**
     * Same as function above, but in 3D. Retrives all points that have hit an obstacle aross a beam.
     * The 3d space is divided into cells to better estimate the space
     * @param beam Beam whose intersection will be estimated
     * @param obstacle_index Index of the measurement that hit the obstacle
     * @param ndiv_horiz Number of divisions in the horizontal plane to use
     * @param ndiv_vert Number of divisions in the vertical plane to use
     * @return Vector of points that belong to the beam that have hit the obstacle
     */
    std::vector<Vector3<>> getBeamEndpoints3D(const Beam* beam, size_t obstacle_index, const unsigned &ndiv_horiz=1, const unsigned &ndiv_vert=1) const;

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
