#ifndef SLAM_PROJECT_SCAN_H
#define SLAM_PROJECT_SCAN_H

#include <ostream>
#include <utility>
#include <rapidjson/document.h>

/* A Scan has multiple sweeps. Each sweep has multiple beams (around 360º) */
class Beam {
private:
  uint8_t* intensities; // This points to the respective intensity in the Sweep Vector
  size_t beam_len;
  double time;
  double angle;

public:
  ~Beam() {
    intensities = nullptr;
  }

  Beam(uint8_t* intensities, size_t beam_len, double time, double angle) : intensities(intensities), beam_len(beam_len),
                                                                           time(time), angle(angle) {}

  [[nodiscard]] uint8_t* getIntensities() const { return intensities; }

  [[nodiscard]] double getTime() const { return time; }

  [[nodiscard]] double getAngle() const { return angle; }

  friend std::ostream& operator<<(std::ostream& os, const Beam& beam);

  static Beam* importJson(const rapidjson::Value& b, uint8_t* intensities);
};

class Sweep {
private:
  uint8_t *intensities;
  size_t sweep_no;
  size_t beam_len;
  std::vector<const Beam*> beams;

public:
  ~Sweep() {
    for (const Beam* beam: beams)
      delete beam;
    delete []intensities;
    intensities = nullptr;
  }

  // Fill intensities with black cells
  Sweep(size_t sweepNo, size_t beamNo, size_t beamLen) : sweep_no(sweepNo), beam_len(beamLen) {
    intensities = new uint8_t [beamLen * beamNo];
  }

  [[nodiscard]] std::vector<const Beam*> getBeams() const { return beams; }

  [[nodiscard]] uint8_t *getIntensities() { return intensities; }

  [[nodiscard]] size_t getBeamLen() const { return beam_len; }

  void addBeam(const Beam* beam) { beams.push_back(beam); }

  void display(const bool &inCartesian);

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

#endif //SLAM_PROJECT_SCAN_H
