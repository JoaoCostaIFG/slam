#ifndef SLAM_PROJECT_SCAN_H
#define SLAM_PROJECT_SCAN_H

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
#define INTENSITY_THRESHOLD 100u
/** Using the range to first feature method, the minimum difference between two
 * intensities for them to be considered an obstacle  **/
#define OBSTACLE_THRESHOLD 20u // 60 %
/** The maximum intensity that a beam intensity may have **/
#define MAX_INTENSITY 255u

namespace sonar {
  /* Our dataset comprises a scan. A Scan has multiple sweeps. Each sweep, composed of multiple beams, has information
   * of the environment around the sonar (in a 360ª radius). Each beam is sent in an angle, and has an array of
   * intensities which correspond to the measurements made along the beam. */
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
    Beam(uint8_t* row, size_t beam_len, double step_dist, double time, double angle) : row(row), beam_len(beam_len),
                                                                                       step_dist(step_dist), time(time),
                                                                                       angle(angle) {}

    /**
     * Yields the first obstacle found by using the simple threshold method. It uses OBSTACLE_THRESHOLD as a threshold.
     * @return the index of the measurement perceived as an obstacle
     */
    [[nodiscard]] size_t getObstacleST() const;

    [[nodiscard]] uint8_t* getIntensities() const { return row; }

    [[nodiscard]] double getTime() const { return time; }

    double getStepDist() const {
      return step_dist;
    }

    [[nodiscard]] double getAngle() const { return angle; }

    friend std::ostream& operator<<(std::ostream& os, const Beam& beam);


    /**
     * Converts a measurement to cartesian coordinates using the beam angle
     * @param i The index of the measurment
     * @return the coordinates of the measurement in cartesian space
     */
    [[nodiscard]] Vector3<> measurementToCartesian(size_t i) const {
      double angle_rad = ((angle + 180) * CV_PI) / 180;
      return this->measurementToCartesian(i, angle_rad);
    }

    /**
     * Converts a measurement to cartesian coordinates, across a given angle
     * @param i The index of the measurment
     * @param angle_rad The angle of the measurement
     * @return the coordinates of the measurement in cartesian space
     */
    [[nodiscard]] static Vector3<> measurementToCartesian(size_t i, double angle_rad) {
      float x = (double) i * cos(angle_rad);
      float y = (double) i * sin(angle_rad);
      // TODO Move to 3D here
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
    [[nodiscard]] Vector3<> coordToReal(Vector3<> pos) const {
      return Vector3f(pos.x() * this->step_dist, pos.y() * this->step_dist, pos.z() * this->step_dist);
    }

    /**
     * @param i The index of measurement to return
     * @return Returns the intensity of the beam at the given index
     */
    uint8_t& at(int i) { return *(row + i); }

    /**
     * @param i The index of measurement to return
     * @return Returns the intensity of the beam at the given index
     */
    [[nodiscard]] uint8_t& at(int i) const { return *(row + i); }

    /**
     * Creates a beam in the heap from a json node
     * @param beam_json The node which holds the beam to be return
     * @param row The row that was attributed to the beam. The beam will keep a pointer to this row.
     * @param step_dist The distance between measurements of the beam
     * @return The created beam
     */
    static Beam* importJson(const rapidjson::Value& beam_json, cv::Mat& row, double step_dist);
  };

  class Sweep {
  private:
    /** This matrix holds, for each beam, its corresponding intensities (beam X intensities) **/
    cv::Mat intensities;
    /** The id of the sweep **/
    size_t sweep_no;
    /** The length of each beam **/
    size_t beam_len;
    /** The beams that correspond to the beam. Each beam has an angle that goes from 0ª to 360ª **/
    std::vector<const Beam*> beams;

  public:
    ~Sweep() {
      for (const Beam* beam: beams)
        delete beam;
    }

    // Fill intensities with black cells
    Sweep(size_t sweepNo, size_t beamNo, size_t beamLen) : intensities(cv::Mat::zeros(beamNo, beamLen, CV_8U)),
                                                           sweep_no(sweepNo), beam_len(beamLen) {}

    [[nodiscard]] std::vector<const Beam*> getBeams() const { return beams; }

    [[nodiscard]] cv::Mat getIntensities() const { return intensities; }

    size_t getSweepNo() const { return sweep_no; }

    [[nodiscard]] size_t getBeamLen() const { return beam_len; }

    void addBeam(const Beam* beam) { beams.push_back(beam); }

    friend std::ostream& operator<<(std::ostream& os, const Sweep& sweep);


    /**
     * Creates a sweep, and its composing beams, in the heap from a json node
     * @param sweep_json The json node which holds the beam to be return
     * @param sweep_no The unique identifier of the sweep to be created
     * @param beam_no The number of beams that the sweep will have
     * @param beam_len The length of each beam of the sweep
     * @param step_dist The distance between each measurement of the beams of the sweep
     * @return
     */
    static Sweep* importJson(const rapidjson::Value& sweep_json, size_t sweep_no, size_t beam_no, size_t beam_len,
                             double step_dist);
  };

  class Scan {
  private:
    size_t scan_len;
    size_t sweep_len;
    std::vector<Sweep*> sweeps;

  public:
    ~Scan() {
      for (Sweep* s: sweeps)
        delete s;
    }

    Scan(size_t scanLen, size_t sweepLen, std::vector<Sweep*> sweeps) :
        scan_len(scanLen),
        sweep_len(sweepLen),
        sweeps(std::move(sweeps)) {}

    [[nodiscard]] const std::vector<Sweep*>& getSweeps() const { return sweeps; }

    friend std::ostream& operator<<(std::ostream& os, const Scan& scan);

    /**
     * Creates a scan, and its corresponding sweeps, in the heap from an input stream
     * @param stream The input stream that holds the json information of the scan
     * @return The parsed scan
     */
    static Scan* importJson(std::istream& stream);
  };
}

#endif //SLAM_PROJECT_SCAN_H
