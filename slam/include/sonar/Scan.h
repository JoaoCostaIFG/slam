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

#define OBSTACLE_THRESHOLD 20
#define MAX_INTENSITY 255

namespace sonar {
  /* A Scan has multiple sweeps. Each sweep has multiple beams (around 360º) */
  class Beam {
  private:
    uchar *row; // This points to the respective intensity in the Sweep Mat
    size_t beam_len;
    double time;
    double angle;

  public:
    Beam(uchar *row, size_t beam_len, double time, double angle) : row(row), beam_len(beam_len),
                          time(time), angle(angle) {}

    /* This returns the index of the measure that corresponds to an obstacle with Simple Threshold */
    [[nodiscard]] size_t getObstacleST() const;

    [[nodiscard]] uchar *getIntensities() const { return row; }

    [[nodiscard]] double getTime() const { return time; }

    [[nodiscard]] double getAngle() const { return angle; }

    friend std::ostream& operator<<(std::ostream& os, const Beam& beam);


    [[nodiscard]] Vector3<> atVec(int i) const {
      double angle_rad = ((angle + 180) * CV_PI) / 180;
      return this->atVec(i, angle_rad);
    }
    [[nodiscard]] Vector3<> atVec(int i, double angle_rad) const {
      float x = (double) i * cos(angle_rad);
      float y = (double) i * sin(angle_rad);
      // TODO Move to 3D here
      return {x, y, 0};
    }

    uchar& at(int i) { return *(row + i); }
    uchar& at(int i) const { return *(row + i); }

    static Beam* importJson(const rapidjson::Value& b, cv::Mat &intensities);
  };

  class Sweep {
  private:
    cv::Mat intensities;
    size_t sweep_no;
    size_t beam_len;
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

    static Sweep* importJson(const rapidjson::Value& s, size_t sweep_no, size_t beam_no, size_t beam_len);
  };

  class Scan {
  private:
    double step_dist;
    size_t scan_len;
    size_t sweep_len;
    std::vector<Sweep*> sweeps;

  public:
    ~Scan() {
      for (Sweep* s: sweeps)
        delete s;
    }

    Scan(size_t scanLen, size_t sweepLen, double step_dist, std::vector<Sweep*> sweeps) : step_dist(step_dist),
                                                                                          scan_len(scanLen),
                                                                                          sweep_len(sweepLen),
                                                                                          sweeps(std::move(sweeps)) {}

    [[nodiscard]] double getStepDist() const { return step_dist; }

    [[nodiscard]] const std::vector<Sweep*>& getSweeps() const { return sweeps; }

    friend std::ostream& operator<<(std::ostream& os, const Scan& scan);

    static Scan* importJson(std::istream& stream);
  };
}

#endif //SLAM_PROJECT_SCAN_H
