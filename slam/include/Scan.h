#ifndef SLAM_PROJECT_SCAN_H
#define SLAM_PROJECT_SCAN_H

#define INTENSITIES_SIZE 399

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <ostream>

using namespace boost::property_tree;


class Beam {
private:
  uint8_t* intensities; // This points to the respective intensity in the Scan array
  double time;
  double angle;

public:
  ~Beam() {
    intensities = nullptr;
  }

  Beam(uint8_t* intensities, double time, double angle) : intensities(intensities), time(time), angle(angle) {}

  [[nodiscard]] uint8_t* getIntensities() const { return intensities; }
  [[nodiscard]] double getTime() const { return time; }
  [[nodiscard]] double getAngle() const { return angle; }

  friend std::ostream& operator<<(std::ostream& os, const Beam& beam);

  static Beam* importJson(const ptree& p, uint8_t *intensities);
};

class Scan {
private:
  double step_dist;
  std::vector<uint8_t> intensities;
  size_t beam_len;
  size_t beam_no;
  std::vector<const Beam*> beams;

public:
  ~Scan() {
    for (const Beam* beam: beams)
      delete beam;
  }

  // Fill intensities with black cells
  Scan(double stepDist, size_t beamLen, size_t beamNo) : step_dist(stepDist), intensities(beamLen * beamNo, 0),
  beam_len(beamLen), beam_no(beamNo) {}

  [[nodiscard]] std::vector<const Beam*> getBeams() const { return beams; }
  [[nodiscard]] double getStepDist() const { return step_dist; }
  [[nodiscard]] std::vector<uint8_t>& getIntensities() { return intensities; }
  [[nodiscard]] size_t getBeamLen() const { return beam_len; }
  [[nodiscard]] size_t getBeamNo() const { return beam_no; }

  void addBeam(const Beam* beam) { beams.push_back(beam); }

  friend std::ostream& operator<<(std::ostream& os, const Scan& scan);

  static Scan* importJson(std::istream& stream);
};


#endif //SLAM_PROJECT_SCAN_H
