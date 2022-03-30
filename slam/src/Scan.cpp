#include <iostream>
#include "../include/Scan.h"

// Scan
Scan* Scan::importJson(std::istream& istream) {
  ptree pt;
  boost::property_tree::read_json(istream, pt);

  double d = pt.get<double>("step_dist");
  int beam_no = pt.get<int>("beam_no");
  int beam_len = pt.get<int>("beam_len");

  // uint8_t *intensities = new uint8_t [beam_len * beam_no];
  Scan* scan = new Scan(d, beam_len, beam_no);
  int i=0;
  for (const auto& b: pt.get_child("beams")) {
    uint8_t *currI = const_cast<uint8_t*>(scan->getIntensities().data());
    Beam* beam = Beam::importJson(b.second, currI + (i * beam_len));
    scan->addBeam(beam);
    ++i;
  }

  return scan;
}

std::ostream& operator<<(std::ostream& os, const Scan& scan) {
  os << "step_dist: " << " beams_count: " << scan.beams.size();
  return os;
}



// Beam
Beam* Beam::importJson(const ptree& p, uint8_t *intensities) {
  double time = p.get<double>("time");
  double angle = p.get<double>("angle");

  const ptree intensities_json = p.get_child("intensities");
  // assert(INTENSITIES_SIZE == intensities_json.size()); This assertion doesn't always apply TODO Fix?
  int cnt = 0;
  for (const auto& i: intensities_json) {
    intensities[cnt] = i.second.get_value<uint8_t>();
    ++cnt;
  }

  return new Beam(intensities, time, angle);
}

std::ostream& operator<<(std::ostream& os, const Beam& beam) {
  os << "intensities: ";
  for (int i = 0; i < INTENSITIES_SIZE; ++i) os << (int) beam.intensities[i] << " ";
  os << std::endl << "\ttime: " << beam.time << " angle: " << beam.angle;
  return os;
}