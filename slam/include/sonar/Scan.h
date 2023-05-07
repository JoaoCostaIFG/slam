#ifndef SLAM_PROJECT_SCAN_H
#define SLAM_PROJECT_SCAN_H

#include <ostream>
#include <opencv2/opencv.hpp>
#include <utility>
#include <rapidjson/document.h>
#include <vector>
#include <cstdint>

#include "../octomap/Vector3.h"
#include "Sweep.h"

using namespace octomap;

/** The maximum intensity that a beam intensity may have **/
#define MAX_INTENSITY 255u

namespace sonar {
/* Our dataset comprises a scan. A Scan has multiple sweeps. Each sweep, composed of multiple beams, has information
 * of the environment around the sonar (in a 360ª radius). Each beam is sent in an angle, and has an array of
 * intensities which correspond to the measurements made along the beam. */

class Scan {
private:
  size_t scan_len;
  size_t sweep_len;
  std::vector<Sweep*> sweeps;

public:
  ~Scan() {
    for (Sweep* s: sweeps)
      delete s;
  }

  Scan(size_t scanLen, size_t sweepLen, std::vector<Sweep*> sweeps) :
      scan_len(scanLen),
      sweep_len(sweepLen),
      sweeps(std::move(sweeps)) {}

  [[nodiscard]] const std::vector<Sweep*>& getSweeps() const { return sweeps; }

  friend std::ostream& operator<<(std::ostream& os, const Scan& scan);

  /**
   * Creates a scan, and its corresponding sweeps, in the heap from an input stream
   * @param stream The input stream that holds the json information of the scan
   * @return The parsed scan
   */
  static Scan* importJson(std::istream& stream);
};
}

#endif //SLAM_PROJECT_SCAN_H
