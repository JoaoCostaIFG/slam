#include <iostream>
#include <vector>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/document.h>
#include <opencv2/opencv.hpp>

#include "../include/sonar/Scan.h"

using namespace sonar;

namespace sonar {
  // Beam
  Beam* Beam::importJson(const rapidjson::Value& beam_json, cv::Mat& row) {
    double time = beam_json["time"].GetDouble();
    double angle = beam_json["angle"].GetDouble();

    const auto& intensities_json = beam_json["intensities"];
    // assert(INTENSITIES_SIZE == intensities_json.Size()); This verification is done in python
    int cnt = 0;
    // Beam points to its intensities, we need to read from json and update them
    for (const auto& i: intensities_json.GetArray()) {
      //beam->at(cnt) = (uint8_t) i.GetUint();
      row.at<uchar>(cnt) = (uint8_t) i.GetUint();
      ++cnt;
    }

    Beam* beam = new Beam(row.ptr(), intensities_json.Size(), time, angle);
    return beam;
  }

  size_t Beam::getObstacleST() const {
    for (size_t i = 0; i + 1 < this->beam_len; ++i) {
      if (this->at(i) > OBSTACLE_THRESHOLD)
        return i + 1;
    }
    return this->beam_len - 1;
  }

  std::ostream& operator<<(std::ostream& os, const Beam& beam) {
    os << "BEAM \tintensities: ";
    size_t n = std::min(10, int(beam.beam_len / 10));
    for (size_t i = 0; i < n; ++i) os << (int) beam.at(i) << " ";
    os << "(...) Len: " << beam.beam_len << std::endl << "\t\ttime: " << beam.time << "\tangle: " << beam.angle;
    return os;
  }

  // Sweep
  Sweep* Sweep::importJson(const rapidjson::Value& sweep_json, size_t sweep_no, size_t beam_no, size_t beam_len) {
    Sweep* sweep = new Sweep(sweep_no, beam_no, beam_len);

    assert(sweep_json.IsArray());
    assert(sweep_json.Size() == beam_no);
    int i = 0;
    for (const auto& b: sweep_json.GetArray()) {
      cv::Mat row = sweep->intensities.row(i);
      Beam* beam = Beam::importJson(b, row);
      sweep->addBeam(beam);
      ++i;
    }

    return sweep;
  }

  std::ostream& operator<<(std::ostream& os, const Sweep& sweep) {
    os << "SWEEP " << sweep.sweep_no << "\tbeams_count: " << sweep.beams.size();
    return os;
  }


  // Scan
  Scan* Scan::importJson(std::istream& stream) {
    rapidjson::Document doc;
    rapidjson::IStreamWrapper istream(stream);
    doc.ParseStream(istream);

    assert(doc.IsObject());

    size_t scan_len = doc["scan_len"].GetInt();
    size_t sweep_len = doc["sweep_len"].GetInt();
    size_t beam_len = doc["beam_len"].GetInt();
    double step_dist = doc["step_dist"].GetDouble();

    const rapidjson::Value& sweeps = doc["sweeps"];
    assert(sweeps.IsArray());
    assert(sweeps.Size() == scan_len);
    std::vector<Sweep*> sweeps_vec;
    sweeps_vec.reserve(scan_len);
    size_t i = 0;
    for (const rapidjson::Value& sweep_json: sweeps.GetArray()) {
      Sweep* sweep = Sweep::importJson(sweep_json, i, sweep_len, beam_len);
      std::cout << *sweep->getBeams().at(0) << std::endl;
      sweeps_vec.push_back(sweep);
      ++i;
    }

    return new Scan(scan_len, sweep_len, step_dist, sweeps_vec);
  }

  std::ostream& operator<<(std::ostream& os, const Scan& scan) {
    os << "SCAN\tlength: " << scan.scan_len << "\tsweep_len: " << scan.sweep_len << "\tstep_dist: " << scan.step_dist;
    return os;
  }
}
