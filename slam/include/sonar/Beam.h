//
// Created by tiago on 07/05/23.
//

#ifndef SLAM_PROJECT_BEAM_H
#define SLAM_PROJECT_BEAM_H

#include <ostream>
#include <opencv2/opencv.hpp>
#include <utility>
#include <rapidjson/document.h>
#include <vector>
#include <cstdint>

#include "../octomap/Vector3.h"

using namespace octomap;

/** Using the range to first feature method, the minimum intensity that a cell
 * can have to be considered for the differential threshold phase **/
#define INTENSITY_THRESHOLD 128u
/** Using the range to first feature method, the minimum difference between two
 * intensities for them to be considered an obstacle  **/
#define OBSTACLE_THRESHOLD 40u // 60 %

namespace sonar {

  class Beam {
  private:
    /** The information of all intensities is stored in the sweep.
     * A beam only needs to point to its row in that matrix. **/
    uint8_t* row;
    /** The length of the intensities that the beam has. Should be the same for all beams **/
    size_t beam_len;
    /** Distance of the step between each intensity of the beam **/
    double step_dist;
    /** Time at which the beam was sent **/
    double time;
    /** Angle that the beam was sent to **/
    double angle;

  public:
    Beam(uint8_t* row, size_t beam_len, double step_dist, double time, double angle)
        : row(row), beam_len(beam_len),
          step_dist(step_dist), time(time),
          angle(angle) {}

    /**
     * Yields the first obstacle found by using the simple threshold method. It uses OBSTACLE_THRESHOLD as a threshold.
     * @return the index of the measurement perceived as an obstacle
     */
    [[nodiscard]] size_t getObstacleST() const;

    [[nodiscard]] uint8_t* getIntensities() const { return row; }

    [[nodiscard]] double getTime() const { return time; }

    [[nodiscard]] double getStepDist() const {
      return step_dist;
    }

    [[nodiscard]] double getAngle() const { return angle; }

    friend std::ostream& operator<<(std::ostream& os, const Beam& beam);

    /**
     * Yields the point in the center of a beam wave of a given measurement
     * @param index Index of the measurement
     * @return Center point in cartesian coordinates
     */
    [[nodiscard]] Vector3<> getCenterPoint(size_t index) const {
      double angle_rad = ((angle + 180) * CV_PI) / 180;
      return sonar::Beam::measurementToCartesian(index, angle_rad);
    }

    /**
     * Converts a measurement to cartesian coordinates, across a given angle. The returned coordinates
     * are scaled to step_dist of the beam, so 1 = 1 step_dist
     * @param i The index of the measurement
     * @param angle_rad The angle of the measurement
     * @return the coordinates of the measurement in cartesian space
     */
    [[nodiscard]] static Vector3<> measurementToCartesian(size_t i, double angle_rad) {
      double x = (double) i * cos(angle_rad);
      double y = (double) i * sin(angle_rad);
      return {x, y, 0};
    }

    /**
     * During the filtering process, the coordinates used don't take into account real world distances to minimize
     * floating point errors. Instead, they consider that the distance between each step is 1. This makes it so
     * that in order to represent the data accurately, the coordinates need to be converted to 'real space'.
     * This is pretty simple (we only need to multiply by step_dist)
     * @param pos The coordinates, represented at the beam's scale, to be converted
     * @return The coordinates converted in real scale
     */
    [[nodiscard]] Vector3<> coordToReal(const Vector3<>& pos) const {
      return {pos.x() * this->step_dist, pos.y() * this->step_dist,
              pos.z() * this->step_dist};
    }

    /**
     * @param i The index of measurement to return
     * @return Returns the intensity of the beam at the given index
     */
    uint8_t& at(size_t i) { return *(row + i); }

    /**
     * @param i The index of measurement to return
     * @return Returns the intensity of the beam at the given index
     */
    [[nodiscard]] uint8_t& at(size_t i) const { return *(row + i); }

    /**
     * Creates a beam in the heap from a json node
     * @param beam_json The node which holds the beam to be return
     * @param row The row that was attributed to the beam. The beam will keep a pointer to this row.
     * @param step_dist The distance between measurements of the beam
     * @return The created beam
     */
    static Beam*
    importJson(const rapidjson::Value& beam_json, cv::Mat& row, double step_dist);
  };

}

#endif //SLAM_PROJECT_BEAM_H
