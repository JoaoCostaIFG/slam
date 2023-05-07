#include <iostream>
#include <vector>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/document.h>
#include <opencv2/opencv.hpp>

#include "../../include/sonar/Beam.h"

using namespace sonar;

namespace sonar {

  Beam* Beam::importJson(const rapidjson::Value& beam_json, cv::Mat& row, double step_dist) {
    double time = beam_json["time"].GetDouble();
    double angle = beam_json["angle"].GetDouble();

    const auto& intensities_json = beam_json["intensities"];
    assert(399 == intensities_json.Size()); // This verification is done in python
    int cnt = 0;
    // Beam points to its intensities, we need to read from json and update them
    for (const auto& i: intensities_json.GetArray()) {
      // Ignore first 24 intensities, which are self reflections
      if (cnt > 23) {
        row.at<uint8_t>(cnt) = (uint8_t) i.GetUint();
      }
      ++cnt;
    }

    Beam* beam = new Beam(row.ptr(), intensities_json.Size(), step_dist, time, angle);
    return beam;
  }

  size_t Beam::getObstacleST() const {
    std::vector filteredInts = std::vector<size_t>(this->beam_len, 0);

    // Intensity threshold phase => Ignore cells that have low intensity
    for (size_t i = 0; i < this->beam_len; ++i) {
      if (this->at(i) > INTENSITY_THRESHOLD)
        filteredInts[i] = this->at(i);
    }
    // Edge threshold phase => Detect significant variations between measurements
    for (size_t i = 0; i + 1 < this->beam_len - 1; ++i) {
      size_t diff = filteredInts[i + 1] - filteredInts[i];
      if (diff > OBSTACLE_THRESHOLD)
        return i + 1;
    }

    return this->beam_len - 1;
  }

  std::ostream& operator<<(std::ostream& os, const Beam& beam) {
    os << "BEAM \tintensities: ";
    size_t n = std::min(50, int(beam.beam_len / 10));
    for (size_t i = 0; i < n; ++i) os << (int) beam.at(i) << " ";
    os << "(...) Len: " << beam.beam_len << std::endl << "\t\ttime: " << beam.time << "\tangle: " << beam.angle;
    return os;
  }

}
