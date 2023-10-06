#include <iostream>
#include <vector>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/document.h>
#include <opencv2/opencv.hpp>

#include "../../include/sonar/Scan.h"

using namespace sonar;

namespace sonar {

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
    Sweep* sweep = Sweep::importJson(sweep_json, i, sweep_len, beam_len, step_dist);
    sweeps_vec.push_back(sweep);
    ++i;
  }

  return new Scan(scan_len, sweep_len, sweeps_vec);
}

std::ostream& operator<<(std::ostream& os, const Scan& scan) {
  os << "SCAN\tlength: " << scan.scan_len << "\tsweep_len: " << scan.sweep_len;
  return os;
}

}
