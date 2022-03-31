#include <iostream>
#include <vector>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/document.h>
#include <opencv2/opencv.hpp>

#include "../include/Scan.h"

// Beam
Beam* Beam::importJson(const rapidjson::Value &beam_json, uint8_t *intensities) {
  double time = beam_json["time"].GetDouble();
  double angle = beam_json["angle"].GetDouble();

  const auto &intensities_json = beam_json["intensities"];
  // assert(INTENSITIES_SIZE == intensities_json.Size()); This verification is done in python
  assert(399 == intensities_json.Size());
  int cnt = 0;
  // Beam points to its intensities, we need to read from json and update them
  for (const auto& i: intensities_json.GetArray()) {
    intensities[cnt] = (uint8_t) i.GetUint();
    ++cnt;
  }

  return new Beam(intensities, cnt, time, angle);
}

std::ostream& operator<<(std::ostream& os, const Beam& beam) {
  os << "BEAM \tintensities: ";
  size_t n = std::min(10, int(beam.beam_len / 10));
  for (size_t i = 0; i < n; ++i) os << (int) beam.intensities[i] << " ";
  os << "(...)" << std::endl << "\t\ttime: " << beam.time << "\tangle: " << beam.angle;
  return os;
}

// Sweep
Sweep* Sweep::importJson(const rapidjson::Value &sweep_json, size_t sweep_no, size_t beam_no, size_t beam_len) {
  Sweep* sweep = new Sweep(sweep_no, beam_len, beam_len);

  assert(sweep_json.IsArray());
  assert(sweep_json.Size() == beam_no);
  int i=0;
  uint8_t *currI = sweep->getIntensities();
  for (const auto& b: sweep_json.GetArray()) {
    Beam* beam = Beam::importJson(b, currI + (i * beam_len));
    sweep->addBeam(beam);
    ++i;
  }

  return sweep;
}

std::ostream& operator<<(std::ostream& os, const Sweep& sweep) {
  os << "SWEEP "<< sweep.sweep_no << "\tbeams_count: " << sweep.beams.size();
  return os;
}

/* Opens an openCV window with a sweep scan (polar). Note: Waits for user to press q to close */
void Sweep::display(const bool &inCartesian=false) {
  cv::Mat img;
  if (inCartesian) {
    std::vector<int> vote_intensities(this->beam_len * 2 * this->beam_len * 2, 0);
    std::vector<int> vote_count(this->beam_len * 2 * this->beam_len * 2, 0);
    img = cv::Mat::zeros(this->beam_len * 2, this->beam_len * 2, CV_8U);

    int x0 = beam_len, y0 = beam_len;
    // Gather votes
    for (const Beam* beam: this->beams) {
      double base_angle = beam->getAngle();
      for (double angle=base_angle-1.5; angle < base_angle + 1.601; angle += 0.1) { // TODO Replace 1.601 with angle step
        // 180 => pi
        // deg => rad
        double angle_rad = ((angle + 180) * CV_PI) / 180;
        for (size_t i=0; i<beam_len; ++i) {
          const uint8_t &intensity = beam->getIntensities()[i];
          int x = x0 + round((double) i * cos(angle_rad));
          int y = y0 + round((double) i * sin(angle_rad));
          int pos = y + x * (this->beam_len * 2);
          vote_intensities[pos] += intensity;
          vote_count[pos] += 1;
        }
      }
    }

    for (size_t y=0; y<this->beam_len * 2; y++) {
      for (size_t x=0; x<this->beam_len * 2; x++) {
        int pos = y + x * (this->beam_len * 2);
        int cnt = vote_count[pos];
        if (cnt != 0) {
          uint8_t mean = round(vote_intensities[pos] / (double) cnt);
          img.at<uchar>(y, x) = mean;
        }
      }
    }

  } else { // Polar Coordinates
    img = cv::Mat(this->beams.size(), beam_len, CV_8U, this->intensities);
  }

  cv::namedWindow("Sweep" + std::to_string(this->sweep_no), cv::WINDOW_AUTOSIZE);
  cv::imshow("Sweep" + std::to_string(this->sweep_no), img);

  int k;
  do {
    k = cv::waitKey(0); // Wait for a keystroke in the window
  } while (k != 'q');
}

// Scan
Scan* Scan::importJson(std::istream& stream) {
  rapidjson::Document doc;
  rapidjson::IStreamWrapper istream(stream);
  doc.ParseStream(istream);
  std::cerr << "?" << std::endl;

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
  size_t i = 0;
  for (const rapidjson::Value& sweep_json: sweeps.GetArray()) {
    Sweep *sweep = Sweep::importJson(sweep_json, i, sweep_len, beam_len);
    sweeps_vec.push_back(sweep);
    ++i;
  }

  return new Scan(scan_len, sweep_len, step_dist, sweeps_vec);
}

std::ostream& operator<<(std::ostream& os, const Scan& scan) {
  os << "SCAN\tlength: " << scan.scan_len << "\tsweep_len: " << scan.sweep_len << "\tstep_dist: " << scan.step_dist;
  return os;
}
