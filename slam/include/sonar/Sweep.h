//
// Created by tiago on 07/05/23.
//

#ifndef SLAM_PROJECT_SWEEP_H
#define SLAM_PROJECT_SWEEP_H

#include <ostream>
#include <opencv2/opencv.hpp>
#include <utility>
#include <rapidjson/document.h>
#include <vector>
#include <cstdint>

#include "../octomap/Vector3.h"
#include "Beam.h"

using namespace octomap;


namespace sonar {

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
    Sweep(size_t sweepNo, size_t beamNo, size_t beamLen) : intensities(
        cv::Mat::zeros(beamNo, beamLen, CV_8U)),
                                                           sweep_no(sweepNo),
                                                           beam_len(beamLen) {}

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
    static Sweep*
    importJson(const rapidjson::Value& sweep_json, size_t sweep_no, size_t beam_no,
               size_t beam_len,
               double step_dist);
  };

}


#endif //SLAM_PROJECT_SWEEP_H
