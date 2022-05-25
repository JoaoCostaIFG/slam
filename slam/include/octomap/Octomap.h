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
  template<typename T = uint16_t>
  class Octomap {
  private:
    using Key = OcNodeKey<T>;
    using KeySet = std::unordered_set<Key, typename Key::Hash, typename Key::Cmp>;
    using Node = OcNode<T>;

    /** The max depth of tree */
    const unsigned int depth;
    /** The same represented by a leaf node/voxel (in meters) */
    const double resolution;

    std::vector<double> stepLookupTable;
    Node* rootNode = nullptr;

    /**
     * Helper method that checks whether the root node exists and creates it if necessary.
     * @return True if the root node was just created. False, otherwise.
     */
    bool createRootIfNeeded() {
      if (this->rootNode == nullptr) {
        this->rootNode = new Node();
        return true;
      }
      return false;
    }

  public:
    /**
     * Instantiates an Octomap with the given maximum depth and resolution.
     * Checks if the chosen max depth fits within the chosen Keys type.
     * @param maxDepth The maximum depth to use.
     * @param resolution The resolution to use.
     */
    Octomap(unsigned int maxDepth, double resolution) :
        depth(maxDepth), resolution(resolution) {
      assert(this->depth >= 1);
      assert(this->depth <= Key::size);

      Key::setMaxCoord((int) pow(2, maxDepth - 1));
      Key::setResolution(resolution);

      // pre-calculate step sizes
      this->stepLookupTable.reserve(this->depth + 2);
      for (unsigned int i = 0; i <= this->depth; ++i) {
        // equivalent to: 2^depth * resolution
        this->stepLookupTable[i] = this->resolution * double(1 << (this->depth - i));
      }
      this->stepLookupTable[this->depth + 1] = this->resolution / 2.0;
    }

    explicit Octomap(double resolution) : Octomap(Key::size, resolution) {}

    Octomap() : Octomap(DFLT_RESOLUTION) {}

    ~Octomap() {
      delete this->rootNode;
    }

    /**
     * Calculates the number of nodes in the Octomap.
     * Keep in mind that the number of nodes is calculated recursively each time this is called,
     * @return The number of nodes in the Octomap.
     */
    [[nodiscard]] unsigned int getSize() const {
      if (this->rootNode == nullptr) return 0;
      return this->rootNode->getChildCount() + 1;
    }

    /**
     * Sets the occupancy value of the node/location represented by @param key.
     * @param key The key that represents the target node/location.
     * @param occ The value of occupancy to set.
     * @param lazy Whether or not to lazy eval (default=false).
     * @return Pointer to the updated node.
     */
    Node* setOccupancy(const Key& key, float occ, bool lazy = false) {
      bool createdRoot = this->createRootIfNeeded();
      return this->rootNode->setOccupancy(key, this->depth, occ, lazy, createdRoot);
    }

    /**
     * Sets the occupancy value of the node at the location represented by @param location.
     * @param location The location of the node to set the value.
     * @param occ The value of occupancy to set.
     * @param lazy Whether or not to lazy eval (default=false).
     * @return Pointer to the updated node.
     */
    Node* setOccupancy(const Vector3<>& location, float occ, bool lazy = false) {
      return this->setOccupancy(Key(location), occ, lazy);
    }

    /**
     * Sets the occupancy value of the node/location represented by @param key to 100%.
     * @param key The key that represents the target node/location.
     * @param lazy Whether or not to lazy eval (default=false).
     * @return Pointer to the updated node.
     */
    Node* setFull(const Key& key, bool lazy = false) {
      return this->setOccupancy(key, 1.0, lazy);
    }

    /**
     * Sets the occupancy value of the node at the location represented by @param location to 100%.
     * @param location The location of the node to set the value.
     * @param lazy Whether or not to lazy eval (default=false).
     * @return Pointer to the updated node.
     */
    Node* setFull(const Vector3<>& location, bool lazy = false) {
      return this->setOccupancy(location, 1.0, lazy);
    }

    /**
     * Sets the occupancy value of the node/location represented by @param key to 0%.
     * @param key The key that represents the target node/location.
     * @param lazy Whether or not to lazy eval (default=false).
     * @return Pointer to the updated node.
     */
    Node* setEmpty(const Key& key, bool lazy = false) {
      return this->setOccupancy(key, 0.0, lazy);
    }

    /**
     * Sets the occupancy value of the node at the location represented by @param location to 0%.
     * @param location The location of the node to set the value.
     * @param lazy Whether or not to lazy eval (default=false).
     * @return Pointer to the updated node.
     */
    Node* setEmpty(const Vector3<>& location, bool lazy = false) {
      return this->setOccupancy(location, 0.0, lazy);
    }

    /**
     * Update the log-odds value of the node/location represented by @param key.
     * @param key The key that represents the target node/location.
     * @param logOdds The value of log-odds to use in the update.
     * @param lazy Whether or not to lazy eval (default=false).
     * @return Pointer to the updated node.
     */
    Node* updateLogOdds(const Key& key, float logOdds, bool lazy = false) {
      // We do a search before updating the target node. This small overhead can save a lot
      // of time in the long-run. Note: the search takes O(l) time, where l is the max depth, which is constant.
      // If the node already exists, we can see if the update would change its log-odds. If the node is stable
      // (+/- 0 affected) or the log-odds is 0, the update wouldn't change anything, but we would still need to
      // perform the intermediate node updates: the intermediate nodes wouldn't change, but the check would be performed.
      auto s = this->search(key);
      if (s && !s->wouldChange(logOdds)) return s;

      bool createdRoot = this->createRootIfNeeded();
      return this->rootNode->updateLogOdds(key, this->depth, logOdds, lazy, createdRoot);
    }

    /**
     * Update the occupancy value of the node/location represented by @param key.
     * @param key The key that represents the target node/location.
     * @param occ The value of occupancy to use in the update.
     * @param lazy Whether or not to lazy eval (default=false).
     * @return Pointer to the updated node.
     */
    Node* updateOccupancy(const Key& key, float occ, bool lazy = false) {
      return this->updateLogOdds(key, (float) Node::prob2logodds(occ), lazy);
    }

    /**
     * Sets the occupancy value of the node at the location represented by @param location.
     * @param location The location of the node to update the value.
     * @param occ The value of occupancy to use in the update.
     * @param lazy Whether or not to lazy eval (default=false).
     * @return Pointer to the updated node.
     */
    Node* updateOccupancy(const Vector3<>& location, float occ, bool lazy = false) {
      return this->updateOccupancy(Key(location), occ, lazy);
    }

    /**
     * Fix the Octomap. This should be called after a set of lazy updates.
     * Updates the log-odds value of intermediate nodes and prunes the tree.
     */
    void fix() {
      if (!this->rootNode) return;
      this->rootNode->fix();
    }

    /**
     * Search for the node represented by @param key in the Octomap.
     * @param key The key that represents the target node/location.
     * @return A pointer to the node represented by @param key.
     */
    Node* search(const Key& key) {
      if (this->rootNode == nullptr) return nullptr;
      return this->rootNode->search(key, this->depth);
    }

    /**
     * Search for the node represented by @param location in the Octomap.
     * @param location The location of the node to search
     * @return A pointer to the node represented by @param locatiion.
     */
    Node* search(const Vector3<>& location) {
      if (this->rootNode == nullptr) return nullptr;
      return this->search(Key(location));
    }

    /**
     * Calculates the keys of the nodes traveled by the raycasting algorithm.
     * Algorithm from: "A Fast Voxel Traversal Algorithm for Ray Tracing" by John Amanatides & Andrew Woo.
     * Based on DDA ray casting algorithm for 3D.
     * @param orig The location to start the raycast from.
     * @param end The end location of the raycast.
     * @return A vector containing the keys of the nodes traveled by the raycasting algorithm
     */
    [[nodiscard]] std::vector<Key> rayCast(const Vector3<>& orig, const Vector3<>& end) const {
      std::vector<Key> ray;

      auto coord = Key(orig);
      auto endKey = Key(end);
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
        if (std::isinf(tDelta[i])) [[unlikely]] {
          tMax[i] = std::numeric_limits<double>::max(); // infinity
        } else [[likely]] {
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

    /**
     * Calculates the keys of the nodes traveled by the raycasting algorithm.
     * Uses the Bresenham Line Algorithm.
     * @param orig The location to start the raycast from.
     * @param end The end location of the raycast.
     * @return A vector containing the keys of the nodes traveled by the raycasting algorithm
     */
    [[nodiscard]] std::vector<Key> rayCastBresenham(const Vector3<>& orig, const Vector3<>& end) const {
      std::vector<Key> ray;

      auto coord = Key(orig);
      auto endKey = Key(end);
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

    /**
     * Updates the cells in the Octomap using a raycast.
     * The cells in the way are marked as free and the last cell is marked as occupied according
     * to the @param occ passed.
     * @param orig The location to start the raycast from.
     * @param end The end location of the raycast.
     * @param occ The occupancy value to use in the update of the end cell.
     * @param lazy Whether or not to use lazy eval (default=false).
     */
    void rayCastUpdate(const Vector3<>& orig, const Vector3<>& end, float occ, bool lazy = false) {
      auto ray = this->rayCast(orig, end);
      for (auto& it: ray)
        this->setEmpty(it, lazy);
      this->updateOccupancy(end, occ, lazy);
      if (lazy) this->rootNode->fix();
    }

    /**
     * Calculates a ray for each endpoint in pointcloud (with origin in @param origin).
     * The rays are calculated in parallel and the reported free and occupied nodes for
     * each ray are joint in 2 sets.
     * These sets are processed so each node is only updated once and occupied nodes have priority.
     *
     * @param pointcloud A vector containing the end points of the rays to calculate (1 ray for each).
     * @param origin The origin location of each raycast.
     * @param occ The occupancy to update the end node (occupied) with.
     */
    void pointcloudUpdate(const std::vector<Vector3f>& pointcloud, const Vector3f& origin, float occ) {
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
        occupiedNodesList.at(idx).insert(Key(endpoint));
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

      // update nodes, discarding updates on freenodes that will be set as occupied
      for (const auto& freeNode: freeNodes) {
        if (!occupiedNodes.contains(freeNode)) {
          this->setEmpty(freeNode, true);
        }
      }
      for (auto& endpoint: occupiedNodes) {
        this->updateOccupancy(endpoint, occ);
      }
      this->rootNode->fix();
    }

    /**
     * The same as pointcloudUpdate but first it discretizes the point cloud.
     * This means that if 2 rays would end up on the same end-point (cell), only the
     * first one is inserted into the tree. This can happen with end-points with different coordinates.
     * In some cases, this can improve performance, but can lead to diferent results.
     *
     * @param pointcloud A vector containing the end points of the rays to calculate (1 ray for each).
     * @param origin The origin location of each raycast.
     * @param occ The occupancy value to update the end node with.
     */
    void discretizedPointcloudUpdate(const std::vector<Vector3f>& pointcloud, const Vector3f& origin, float occ) {
      std::vector<Vector3f> discretizedPc;
      KeySet endpoints;
      for (const auto& endpointCoord: pointcloud) {
        Key endpoint = Key(endpointCoord);
        auto succ = endpoints.insert(std::move(endpoint));
        if (succ.second) {
          discretizedPc.push_back(endpointCoord);
        }
      }

      this->pointcloudUpdate(discretizedPc, origin, occ);
    }

    /**
     * Outputs the tree to the given stream.
     * The tree is output in a binary format compatible with [octoviz](https://github.com/OctoMap/octomap/tree/devel/octovis).
     *
     * @param os The stream to output to.
     * @return True on success. False, otherwise.
     */
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

    /**
     * Outputs the tree to a file with the given file name. The file will be truncated before writing.
     * The tree is output in a binary format compatible with [octoviz](https://github.com/OctoMap/octomap/tree/devel/octovis).
     *
     * @param filename The name of the file to output to.
     * @return True on success. False, otherwise.
     */
    bool writeBinary(const std::string& filename) {
      std::ofstream binary_outfile(filename.c_str(), std::ios_base::binary);
      if (!binary_outfile.is_open()) {
        return false;
      }
      return writeBinary(binary_outfile);
    }

    typedef OctomapIterator<Node> iterator;
    typedef const OctomapIterator<Node> const_iterator;

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
