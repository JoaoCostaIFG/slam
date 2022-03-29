#ifndef SLAM_OCTOMAP_H
#define SLAM_OCTOMAP_H

#include <memory>
#include <unordered_set>
#include <vector>

#include "OcNode.h"
#include "Vector3.h"

#define DFLT_MAX_DEPTH 16
#define DFLT_RESOLUTION 0.1

namespace octomap {
  class Octomap {
  private:
    const unsigned int depth;
    const double resolution; // in meters

    std::vector<double> stepLookupTable;
    // number of nodes in the tree (starts with 1 root node)
    OcNode* rootNode;

    bool createRootIfNeeded();

  public:
    Octomap(unsigned int maxDepth, double resolution);

    Octomap();

    ~Octomap();

    [[nodiscard]] unsigned int getSize() const {
      if (this->rootNode == nullptr) return 0;
      return this->rootNode->getChildCount() + 1;
    }

    OcNode* setOccupancy(const OcNodeKey& key, float occ);

    OcNode* setOccupancy(const Vector3<>& location, float occ);

    OcNode* setFull(const OcNodeKey& key) {
      return this->setOccupancy(key, 1.0);
    }

    OcNode* setFull(const Vector3<>& location) {
      return this->setOccupancy(location, 1.0);
    }

    OcNode* setEmpty(const OcNodeKey& key) {
      return this->setOccupancy(key, 0.0);
    }

    OcNode* setEmpty(const Vector3<>& location) {
      return this->setOccupancy(location, 0.0);
    }

    OcNode* updateLogOdds(const OcNodeKey& key, float logOdds);

    OcNode* updateOccupancy(const OcNodeKey& key, float occ);

    OcNode* updateOccupancy(const Vector3<>& location, float occ);

    OcNode* search(const OcNodeKey& key);

    OcNode* search(const Vector3<>& location);

    // Algorithm from "A Fast Voxel Traversal Algorithm for Ray Tracing" by John Amanatides & Andrew Woo.
    // Based on DDA ray casting algorithm for 3D.
    std::vector<std::unique_ptr<OcNodeKey>> rayCast(const Vector3<>& orig, const Vector3<>& end);

    OcNode* rayCastUpdate(const Vector3<>& orig, const Vector3<>& end, float occ);

    // Calculates a ray for each endpoint in pointcloud (with origin in origin).
    // The rays are calculated in parallel and the reported free and occupied nodes for
    // each ray are joint in 2 sets.
    // These sets are processed so each node is only updated once and occupied nodes have priority.
    void pointcloudUpdate(const std::vector<Vector3f>& pointcloud, const Vector3f& origin);

    // The binary format is compatible with octoviz
    // See: https://github.com/OctoMap/octomap/tree/devel/octovis
    bool writeBinary(std::ostream& os);

    bool writeBinary(const std::string& filename);
  };
}

#endif //SLAM_OCTOMAP_H
