#ifndef SLAM_OCTOMAP_H
#define SLAM_OCTOMAP_H

#include <cassert>
#include <unordered_set>
#include <vector>
#include <fstream>

#ifdef _OPENMP

#include <omp.h>

#endif

#include "OcNode.h"
#include "OcNodeKey.h"
#include "OctomapIterator.h"
#include "Vector3.h"

#define DFLT_RESOLUTION 0.1

namespace octomap {
  template<class KEY = OcNodeKey<>>
  class Octomap {
  private:
    using KeySet = std::unordered_set<KEY, typename KEY::Hash, typename KEY::Cmp>;
    using Node = OcNode<KEY>;

    const unsigned int depth;
    const double resolution; // in meters

    std::vector<double> stepLookupTable;
    // number of nodes in the tree (starts with 1 root node)
    Node* rootNode = nullptr;

    bool createRootIfNeeded() {
      if (this->rootNode == nullptr) {
        this->rootNode = new Node();
        return true;
      }
      return false;
    }

  public:
    Octomap(unsigned int maxDepth, double resolution) :
        depth(maxDepth), resolution(resolution) {
      assert(this->depth >= 1);
      assert(this->depth <= KEY::size);

      KEY::setMaxCoord((int) pow(2, maxDepth - 1));
      KEY::setResolution(resolution);

      // pre-calculate step sizes
      this->stepLookupTable.reserve(this->depth + 2);
      for (unsigned int i = 0; i <= this->depth; ++i) {
        // equivalent to: 2^depth * resolution
        this->stepLookupTable[i] = this->resolution * double(1 << (this->depth - i));
      }
      this->stepLookupTable[this->depth + 1] = this->resolution / 2.0;
    }

    explicit Octomap(double resolution) : Octomap(KEY::size, resolution) {}

    Octomap() : Octomap(DFLT_RESOLUTION) {}

    ~Octomap() {
      delete this->rootNode;
    }

    [[nodiscard]] unsigned int getSize() const {
      if (this->rootNode == nullptr) return 0;
      return this->rootNode->getChildCount() + 1;
    }

    Node* setOccupancy(const KEY& key, float occ) {
      bool createdRoot = this->createRootIfNeeded();
      return this->rootNode->setOccupancy(key, this->depth, occ, createdRoot);
    }

    Node* setOccupancy(const Vector3<>& location, float occ) {
      return this->setOccupancy(KEY(location), occ);
    }

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

    Node* updateLogOdds(const KEY& key, float logOdds) {
      // We do a search before updating the target node. This small overhead can save a lot
      // of time in the long-run. Note: the search takes O(l) time, where l is the max depth, which is constant.
      // If the node already exists, we can see if the update would change its log-odds. If the node is stable
      // (+/- 0 affected) or the log-odds is 0, the update wouldn't change anything, but we would still need to
      // perform the intermediate node updates: the intermediate nodes wouldn't change, but the check would be performed.
      auto s = this->search(key);
      if (s && !s->wouldChange(logOdds)) return s;

      bool createdRoot = this->createRootIfNeeded();
      return this->rootNode->updateLogOdds(key, this->depth, logOdds, createdRoot);
    }

    Node* updateOccupancy(const KEY& key, float occ) {
      return this->updateLogOdds(key, (float) Node::prob2logodds(occ));
    }

    Node* updateOccupancy(const Vector3<>& location, float occ) {
      return this->updateOccupancy(KEY(location), occ);
    }

    Node* search(const KEY& key) {
      if (this->rootNode == nullptr) return nullptr;
      return this->rootNode->search(key, this->depth);
    }

    Node* search(const Vector3<>& location) {
      if (this->rootNode == nullptr) return nullptr;
      return this->search(KEY(location));
    }

    // Algorithm from "A Fast Voxel Traversal Algorithm for Ray Tracing" by John Amanatides & Andrew Woo.
    // Based on DDA ray casting algorithm for 3D.
    [[nodiscard]] std::vector<KEY> rayCast(const Vector3<>& orig, const Vector3<>& end) const {
      std::vector<KEY> ray;

      auto coord = KEY(orig);
      auto endKey = KEY(end);
      if (coord == endKey) return ray;

      // Initialization phase
      auto step = Vector3i();
      auto tMax = Vector3d();
      auto tDelta = Vector3d();

      auto direction = (end - orig);
      direction.normalize();
      Vector3 origCoord = coord.toCoord();
      double length = (endKey.toCoord() - origCoord).norm();

      for (int i = 0; i < 3; ++i) {
        if (direction[i] > 0) step[i] = 1;
        else step[i] = -1;

        // It should be 1/abs(direction[i]) from the paper, but out cell size varies by *resolution*
        // so we multiply it.
        tDelta[i] = this->resolution / fabs(direction[i]);
        if (std::isinf(tDelta[i])) {
          tMax[i] = std::numeric_limits<double>::max(); // infinity
        } else {
          double voxelBorder = origCoord[i] + step[i] * this->stepLookupTable[this->depth + 1];
          tMax[i] = (voxelBorder - orig[i]) / direction[i];
        }
      }

      // Incremental phase
      ray.reserve((size_t) length);
      double* min;
      while (coord != endKey &&
             (
                 *(min = std::min_element(tMax.begin(), tMax.end())) <= length ||
                 (coord.toCoord() - origCoord).norm() <= length
             )) {
        int idx = int(min - tMax.begin());
        // save key
        ray.push_back(coord);
        // gen next key
        tMax[idx] += tDelta[idx];
        coord[idx] += step[idx];
      }

      return ray;
    }

    [[nodiscard]] std::vector<KEY> rayCastBresenham(const Vector3<>& orig, const Vector3<>& end) const {
      std::vector<KEY> ray;

      auto coord = KEY(orig);
      auto endKey = KEY(end);
      if (coord == endKey) return ray;

      auto d = Vector3<int>();
      auto d2 = Vector3<int>();
      auto step = Vector3i();
      for (int i = 0; i < 3; ++i) {
        // TODO I don't like these casts to int, but it shouldn't matter unless we're using giant keys
        d[i] = (int) endKey[i] - (int) coord[i];
        step[i] = (d[i] > 0) ? 1 : -1;
        d[i] = abs(d[i]);
        d2[i] = 2 * d[i];
      }

      int p1, p2;
      int* max = std::max_element(d.begin(), d.end());
      int idx = int(max - d.begin());
      int idx1 = (idx + 1) % 3;
      int idx2 = (idx + 2) % 3;

      p1 = d2[idx1] - d[idx];
      p2 = d2[idx2] - d[idx];

      ray.reserve(d[0] + d[1] + d[2]);
      while (coord[idx] != endKey[idx]) {
        // save coord
        ray.push_back(coord);
        // new coord
        coord[idx] += step[idx];
        if (p1 >= 0) {
          coord[idx1] += step[idx1];
          p1 -= d2[idx];
        }
        if (p2 >= 0) {
          coord[idx2] += step[idx2];
          p2 -= d2[idx];
        }
        p1 += d2[idx1];
        p2 += d2[idx2];
      }

      return ray;
    }

    Node* rayCastUpdate(const Vector3<>& orig, const Vector3<>& end, float occ) {
      auto ray = this->rayCast(orig, end);
      for (auto& it: ray)
        this->setEmpty(it);
      return this->updateOccupancy(end, occ);
    }

    // Calculates a ray for each endpoint in pointcloud (with origin in origin).
    // The rays are calculated in parallel and the reported free and occupied nodes for
    // each ray are joint in 2 sets.
    // These sets are processed so each node is only updated once and occupied nodes have priority.
    void pointcloudUpdate(const std::vector<Vector3f>& pointcloud, const Vector3f& origin) {
      std::vector<KeySet> freeNodesList, occupiedNodesList;

      // small hack to alloc 2 containers for each simultaneous thread
#ifdef _OPENMP
#pragma omp parallel default(none) shared(pointcloud, freeNodesList, occupiedNodesList)
#pragma omp critical
      {
        if (omp_get_thread_num() == 0) {
          int threadCnt = omp_get_num_threads();
          freeNodesList.resize(threadCnt);
          occupiedNodesList.resize(threadCnt);
          for (int i = 0; i < threadCnt; ++i) {
            freeNodesList.at(i).reserve((pointcloud.size() / threadCnt) * 50);
            occupiedNodesList.at(i).reserve(pointcloud.size() / threadCnt);
          }
        }
      }
#else
      freeNodesList.resize(1);
  occupiedNodesList.resize(1);
  freeNodesList.at(0).reserve(pointcloud.size() * 50);
  occupiedNodesList.at(0).reserve(pointcloud.size());
#endif

#ifdef _OPENMP
#pragma omp parallel for schedule(auto) default(none) shared(pointcloud, origin, freeNodesList, occupiedNodesList)
#endif
      for (const auto& endpoint: pointcloud) {
        int idx = 0;
#ifdef _OPENMP
        idx = omp_get_thread_num();
#endif
        // cast the ray
        //auto ray = this->rayCast(origin, endpoint);
        auto ray = this->rayCastBresenham(origin, endpoint);
        // store the ray info
        freeNodesList.at(idx).insert(ray.begin(), ray.end());
        occupiedNodesList.at(idx).insert(KEY(endpoint));
      }

      // join measurements
      KeySet occupiedNodes;
      for (auto& occupiedNodesI: occupiedNodesList) {
        occupiedNodes.merge(occupiedNodesI);
      }
      KeySet freeNodes;
      for (auto& freeNodesI: freeNodesList) {
        freeNodes.merge(freeNodesI);
      }

      // TODO these loops could benefit from lazy eval!
      // update nodes, discarding updates on freenodes that will be set as occupied
      for (const auto& freeNode: freeNodes) {
        if (!occupiedNodes.contains(freeNode)) {
          this->setEmpty(freeNode);
        }
      }
      for (auto& endpoint: occupiedNodes) {
        this->setFull(endpoint);
      }
    }

    // The same as pointcloudUpdate but first it discretizes the point cloud. This means that
    // if 2 rays would end up on the same end-point (cell), only the first one is inserted into the tree.
    // In some cases, this can improve performance, but can lead to diferente results.
    void discretizedPointcloudUpdate(const std::vector<Vector3f>& pointcloud, const Vector3f& origin) {
      std::vector<Vector3f> discretizedPc;
      KeySet endpoints;
      for (const auto& endpointCoord: pointcloud) {
        KEY endpoint = KEY(endpointCoord);
        auto succ = endpoints.insert(std::move(endpoint));
        if (succ.second) {
          discretizedPc.push_back(endpointCoord);
        }
      }

      this->pointcloudUpdate(discretizedPc, origin);
    }

    // The binary format is compatible with octoviz
    // See: https://github.com/OctoMap/octomap/tree/devel/octovis
    bool writeBinary(std::ostream& os) {
      os << "# Octomap OcTree binary file\n";
      os << "id OcTree\n";
      os << "size " << this->getSize() << std::endl;
      os << "res " << this->resolution << std::endl;
      os << "data" << std::endl;
      if (this->rootNode == nullptr) return false;
      this->rootNode->writeBinary(os);
      return true;
    }

    bool writeBinary(const std::string& filename) {
      std::ofstream binary_outfile(filename.c_str(), std::ios_base::binary);
      if (!binary_outfile.is_open()) {
        return false;
      }
      return writeBinary(binary_outfile);
    }

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
