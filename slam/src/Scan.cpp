#include <iostream>
#include <vector>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/document.h>

#include "../include/Scan.h"

// Sweep
Sweep* Sweep::importJson(const rapidjson::Value &sweep_json, size_t beam_no, size_t beam_len) {
  Sweep* sweep = new Sweep(beam_len, beam_no);

  assert(sweep_json.IsArray());
  int i=0;
  for (const auto& b: sweep_json.GetArray()) {
    uint8_t *currI = const_cast<uint8_t*>(sweep->getIntensities().data());
    Beam* beam = Beam::importJson(b, currI + (i * beam_len));
    sweep->addBeam(beam);
    ++i;
  }

  return sweep;
}

std::ostream& operator<<(std::ostream& os, const Sweep& sweep) {
  os << "SWEEP\tbeams_count: " << sweep.beams.size();
  return os;
}

// Beam
Beam* Beam::importJson(const rapidjson::Value &beam_json, uint8_t *intensities) {
  double time = beam_json["time"].GetDouble();
  double angle = beam_json["angle"].GetDouble();

  const auto &intensities_json = beam_json["intensities"];
  // assert(INTENSITIES_SIZE == intensities_json.Size()); This verification is done in python
  int cnt = 0;
  for (const auto& i: intensities_json.GetArray()) {
    intensities[cnt] = (uint8_t) i.GetUint();
    ++cnt;
  }

  return new Beam(intensities, cnt, time, angle);
}

std::ostream& operator<<(std::ostream& os, const Beam& beam) {
  os << "BEAM \tintensities: ";
  size_t n = std::min(10, int(beam.beam_len / 10));
  for (int i = 0; i < n; ++i) os << (int) beam.intensities[i] << " ";
  os << "(...)" << std::endl << "\t\ttime: " << beam.time << "\tangle: " << beam.angle;
  return os;
}

Scan* Scan::importJson(std::istream& stream) {
  rapidjson::Document doc;
  rapidjson::IStreamWrapper istream(stream);
  doc.ParseStream(istream);

  assert(doc.IsObject());

  size_t scan_len = doc["scan_len"].GetInt();
  size_t sweep_len = doc["sweep_len"].GetInt();
  size_t beam_len = doc["beam_len"].GetInt();
  double step_dist = doc["step_dist"].GetDouble();

  const rapidjson::Value & sweeps = doc["sweeps"];
  assert(sweeps.IsArray());
  assert(sweeps.Size() == scan_len);
  std::vector<Sweep*> sweeps_vec;
  sweeps_vec.reserve(scan_len);
  for (const rapidjson::Value& sweep_json: sweeps.GetArray()) {
    Sweep *sweep = Sweep::importJson(sweep_json, sweep_len, beam_len);
    sweeps_vec.push_back(sweep);
  }

  return new Scan(scan_len, sweep_len, step_dist, sweeps_vec);
}

std::ostream& operator<<(std::ostream& os, const Scan& scan) {
  os << "SCAN\tsweep_no: " << scan.sweep_no << "\tbeam_no: " << scan.beam_no << "\tstep_dist: " << scan.step_dist;
  return os;
}
