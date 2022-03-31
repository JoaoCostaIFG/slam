#ifndef SLAM_OCTOMAP_H
#define SLAM_OCTOMAP_H

#include <cassert>
#include <memory>
#include <unordered_set>
#include <vector>

#include "OcNode.h"
#include "OcNodeKey.hxx"
#include "OctomapIterator.h"
#include "Vector3.h"

#define DFLT_MAX_DEPTH 16
#define DFLT_RESOLUTION 0.1

namespace octomap {
  template<class KEY>
  class Octomap {
  private:
    using KeySet = std::unordered_set<KEY, typename KEY::Hash, typename KEY::Cmp>;
    using Node = OcNode<KEY>;

    const unsigned int depth;
    const double resolution; // in meters

    std::vector<double> stepLookupTable;
    // number of nodes in the tree (starts with 1 root node)
    Node* rootNode;

    bool createRootIfNeeded();

  public:
    Octomap(unsigned int maxDepth, double resolution) :
        depth(maxDepth), resolution(resolution) {
      assert(this->depth >= 1);
      assert(this->depth <= KEY::size());

      KEY::setMaxCoord((int) pow(2, maxDepth - 1));
      KEY::setResolution(resolution);

      this->rootNode = nullptr;

      // pre-calculate step sizes
      this->stepLookupTable.reserve(this->depth + 2);
      for (unsigned int i = 0; i <= this->depth; ++i) {
        // equivalent to: 2^depth * resolution
        this->stepLookupTable[i] = this->resolution * double(1 << (this->depth - i));
      }
      this->stepLookupTable[this->depth + 1] = this->resolution / 2.0;
    }

    Octomap() : Octomap(DFLT_MAX_DEPTH, DFLT_RESOLUTION) {
      std::cout << this->depth << std::endl;
    }

    //~Octomap() {
    //  delete this->rootNode;
    //}

    [[nodiscard]] unsigned int getSize() const {
      if (this->rootNode == nullptr) return 0;
      return this->rootNode->getChildCount() + 1;
    }

    Node* setOccupancy(const KEY& key, float occ);

    Node* setOccupancy(const Vector3<>& location, float occ);

    Node* setFull(const KEY& key) {
      return this->setOccupancy(key, 1.0);
    }

    Node* setFull(const Vector3<>& location) {
      return this->setOccupancy(location, 1.0);
    }

    Node* setEmpty(const KEY& key) {
      return this->setOccupancy(key, 0.0);
    }

    Node* setEmpty(const Vector3<>& location) {
      return this->setOccupancy(location, 0.0);
    }

    Node* updateLogOdds(const KEY& key, float logOdds);

    Node* updateOccupancy(const KEY& key, float occ);

    Node* updateOccupancy(const Vector3<>& location, float occ);

    Node* search(const KEY& key);

    Node* search(const Vector3<>& location);

    // Algorithm from "A Fast Voxel Traversal Algorithm for Ray Tracing" by John Amanatides & Andrew Woo.
    // Based on DDA ray casting algorithm for 3D.
    [[nodiscard]] std::vector<KEY> rayCast(const Vector3<>& orig, const Vector3<>& end) const;

    [[nodiscard]] std::vector<KEY> rayCastBresenham(const Vector3<>& orig, const Vector3<>& end) const;

    Node* rayCastUpdate(const Vector3<>& orig, const Vector3<>& end, float occ);

    // Calculates a ray for each endpoint in pointcloud (with origin in origin).
    // The rays are calculated in parallel and the reported free and occupied nodes for
    // each ray are joint in 2 sets.
    // These sets are processed so each node is only updated once and occupied nodes have priority.
    void pointcloudUpdate(const std::vector<Vector3f>& pointcloud, const Vector3f& origin);

    // The same as pointcloudUpdate but first it discretizes the point cloud. This means that
    // if 2 rays would end up on the same end-point (cell), only the first one is inserted into the tree.
    // In some cases, this can improve performance, but can lead to diferente results.
    void discretizedPointcloudUpdate(const std::vector<Vector3f>& pointcloud, const Vector3f& origin);

    // The binary format is compatible with octoviz
    // See: https://github.com/OctoMap/octomap/tree/devel/octovis
    bool writeBinary(std::ostream& os);

    bool writeBinary(const std::string& filename);

    typedef OctomapIterator<KEY> iterator;
    typedef const OctomapIterator<KEY> const_iterator;

    iterator begin() {
      return iterator(this->rootNode);
    }

    iterator end() {
      return iterator(nullptr);
    }

    [[nodiscard]] const_iterator begin() const {
      return const_iterator(this->rootNode);
    }

    [[nodiscard]] const_iterator end() const {
      return const_iterator(nullptr);
    }
  };
}

#endif //SLAM_OCTOMAP_H
