#include <iostream>
#include <vector>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/document.h>
#include <opencv2/opencv.hpp>

#include "../../include/sonar/Sweep.h"

using namespace sonar;

namespace sonar {

  Sweep* Sweep::importJson(const rapidjson::Value& sweep_json, size_t sweep_no, size_t beam_no, size_t beam_len,
                           double step_dist) {
    Sweep* sweep = new Sweep(sweep_no, beam_no, beam_len);

    assert(sweep_json.IsArray());
    assert(sweep_json.Size() == beam_no);
    int i = 0;
    for (const auto& b: sweep_json.GetArray()) {
      cv::Mat row = sweep->intensities.row(i);
      Beam* beam = Beam::importJson(b, row, step_dist);
      sweep->addBeam(beam);
      ++i;
    }

    return sweep;
  }

  std::ostream& operator<<(std::ostream& os, const Sweep& sweep) {
    os << "SWEEP " << sweep.sweep_no << "\tbeams_count: " << sweep.beams.size();
    return os;
  }

}
