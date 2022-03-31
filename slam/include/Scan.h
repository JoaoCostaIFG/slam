#ifndef SLAM_PROJECT_SCAN_H
#define SLAM_PROJECT_SCAN_H

#define INTENSITIES_SIZE 399

#include <ostream>
#include <utility>
#include <rapidjson/document.h>

class Beam {
private:
  uint8_t* intensities; // This points to the respective intensity in the Sweep array
  size_t beam_len;
  double time;
  double angle;

public:
  ~Beam() {
    intensities = nullptr;
  }

  Beam(uint8_t* intensities, size_t beam_len, double time, double angle) : intensities(intensities), time(time),
    angle(angle), beam_len(beam_len) {}

  [[nodiscard]] uint8_t* getIntensities() const { return intensities; }
  [[nodiscard]] double getTime() const { return time; }
  [[nodiscard]] double getAngle() const { return angle; }

  friend std::ostream& operator<<(std::ostream& os, const Beam& beam);

  static Beam* importJson(const rapidjson::Value &b, uint8_t *intensities);
};

class Sweep {
private:
  std::vector<uint8_t> intensities;
  size_t beam_no;
  size_t beam_len;
  std::vector<const Beam*> beams;

public:
  ~Sweep() {
    for (const Beam* beam: beams)
      delete beam;
  }

  // Fill intensities with black cells
  Sweep(size_t beamNo, size_t beamLen) : intensities(beamLen * beamNo, 0), beam_no(beamNo), beam_len(beamLen) {}

  [[nodiscard]] std::vector<const Beam*> getBeams() const { return beams; }
  [[nodiscard]] std::vector<uint8_t>& getIntensities() { return intensities; }
  [[nodiscard]] size_t getBeamNo() const { return beam_no; }
  [[nodiscard]] size_t getBeamLen() const { return beam_len; }

  void addBeam(const Beam* beam) { beams.push_back(beam); }

  friend std::ostream& operator<<(std::ostream& os, const Sweep& sweep);

  static Sweep* importJson(const rapidjson::Value &s, size_t beam_no, size_t beam_len);
};

class Scan {
private:
  double step_dist;
  size_t sweep_no;
  size_t beam_no;
  std::vector<Sweep*> sweeps;

public:
  ~Scan() {
    for (Sweep *s: sweeps)
      delete s;
  }
  Scan(size_t sweepNo, size_t beamNo, double step_dist, std::vector<Sweep*> sweeps) : sweep_no(sweepNo),
                                                                                           beam_no(beamNo), step_dist(step_dist),
                                                                                           sweeps(std::move(sweeps)) {}

  [[nodiscard]] double getStepDist() const { return step_dist; }
  const std::vector<Sweep*>& getSweeps() const { return sweeps; }

  friend std::ostream& operator<<(std::ostream& os, const Scan& scan);

  static Scan* importJson(std::istream& stream);
};


#endif //SLAM_PROJECT_SCAN_H
