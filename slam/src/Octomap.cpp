#include <cmath>
#include <iostream>
#include <fstream>

#ifdef _OPENMP

#include <omp.h>

#endif

#include "../include/octomap/Octomap.h"

using namespace octomap;

template<class KEY>
bool Octomap<KEY>::createRootIfNeeded() {
  if (this->rootNode == nullptr) {
    this->rootNode = new Node();
    return true;
  }
  return false;
}

template<class KEY>
OcNode<KEY>* Octomap<KEY>::setOccupancy(const KEY& key, float occ) {
  bool createdRoot = this->createRootIfNeeded();
  return this->rootNode->setOccupancy(key, this->depth, occ, createdRoot);
}

template<class KEY>
OcNode<KEY>* Octomap<KEY>::setOccupancy(const Vector3<>& location, const float occ) {
  return this->setOccupancy(KEY(location), occ);
}

template<class KEY>
OcNode<KEY>* Octomap<KEY>::updateLogOdds(const KEY& key, const float logOdds) {
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

template<class KEY>
OcNode<KEY>* Octomap<KEY>::updateOccupancy(const KEY& key, const float occ) {
  return this->updateLogOdds(key, (float) Node::prob2logodds(occ));
}

template<class KEY>
OcNode<KEY>* Octomap<KEY>::updateOccupancy(const Vector3<>& location, const float occ) {
  return this->updateOccupancy(KEY(location), occ);
}

template<class KEY>
OcNode<KEY>* Octomap<KEY>::search(const KEY& key) {
  if (this->rootNode == nullptr) return nullptr;
  return this->rootNode->search(key, this->depth);
}

template<class KEY>
OcNode<KEY>* Octomap<KEY>::search(const Vector3<>& location) {
  if (this->rootNode == nullptr) return nullptr;
  return this->search(KEY(location));
}

template<class KEY>
std::vector<KEY> Octomap<KEY>::rayCast(const Vector3<>& orig, const Vector3<>& end) const {
  std::vector<KEY> ray;

  auto coord = KEY(orig);
  auto endKey = KEY(end);
  if (coord == endKey) return ray;

  Vector3 origCoord = coord->toCoord();
  Vector3 endCoord = endKey->toCoord();

  // Initialization phase
  auto step = Vector3i();
  auto tMax = Vector3d();
  auto tDelta = Vector3d();

  auto direction = (end - orig);
  direction.normalize();

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
  double length = (endCoord - origCoord).norm();
  ray.reserve((size_t) length);
  double* min;
  while (*coord != *endKey &&
         (
             *(min = std::min_element(tMax.begin(), tMax.end())) <= length ||
             (coord->toCoord() - origCoord).norm() <= length
         )) {
    int idx = int(min - tMax.begin());
    // save key
    ray.push_back(coord);
    // gen next key
    tMax[idx] += tDelta[idx];
    coord->set(idx, coord->get(idx) + step[idx]);
  }

  return ray;
}


template<class KEY>
std::vector<KEY> Octomap<KEY>::rayCastBresenham(const Vector3<>& orig, const Vector3<>& end) const {
  std::vector<KEY> ray;

  auto coord = KEY(orig);
  auto endKey = KEY(end);
  if (coord == endKey) return ray;

  auto d = Vector3<int>();
  auto d2 = Vector3<int>();
  auto step = Vector3i();
  for (int i = 0; i < 3; ++i) {
    // TODO I don't like these casts to int, but it shouldn't matter unless we're using giant keys
    d[i] = (int) endKey->get(i) - (int) coord->get(i);
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
  while (coord->get(idx) != endKey->get(idx)) {
    // save coord
    ray.push_back(coord);
    // new coord
    coord->set(idx, coord->get(idx) + step[idx]);
    if (p1 >= 0) {
      coord->set(idx1, coord->get(idx1) + step[idx1]);
      p1 -= d2[idx];
    }
    if (p2 >= 0) {
      coord->set(idx2, coord->get(idx2) + step[idx2]);
      p2 -= d2[idx];
    }
    p1 += d2[idx1];
    p2 += d2[idx2];
  }

  return ray;
}

template<class KEY>
OcNode<KEY>* Octomap<KEY>::rayCastUpdate(const Vector3<>& orig, const Vector3<>& end, float occ) {
  auto ray = this->rayCast(orig, end);
  for (auto& it: ray)
    this->setEmpty(*it);
  return this->updateOccupancy(end, occ);
}

template<class KEY>
void Octomap<KEY>::pointcloudUpdate(const std::vector<Vector3f>& pointcloud, const Vector3f& origin) {
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
    auto& freeNodes = freeNodesList.at(idx);
    for (auto& rayPoint: ray) {
      freeNodes.insert(std::move(rayPoint));
    }
    occupiedNodesList.at(idx).insert(KEY(endpoint));
  }

  //// join measurements
  //KeySet occupiedNodes;
  //for (auto& occupiedNodesI: occupiedNodesList) {
  //  occupiedNodes.merge(occupiedNodesI);
  //}
  //KeySet freeNodes;
  //for (auto& freeNodesI: freeNodesList) {
  //  freeNodes.merge(freeNodesI);
  //}

  //// TODO these loops could benefit from lazy eval!
  //// update nodes, discarding updates on freenodes that will be set as occupied
  //for (const auto& freeNode: freeNodes) {
  //  if (!occupiedNodes.contains(freeNode)) {
  //    this->setEmpty(*freeNode);
  //  }
  //}
  //for (auto& endpoint: occupiedNodes) {
  //  this->setFull(*endpoint);
  //}
}

template<class KEY>
void Octomap<KEY>::discretizedPointcloudUpdate(const std::vector<Vector3f>& pointcloud, const Vector3f& origin) {
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

template<class KEY>
bool Octomap<KEY>::writeBinary(std::ostream& os) {
  os << "# Octomap OcTree binary file\n";
  os << "id OcTree\n";
  os << "size " << this->getSize() << std::endl;
  os << "res " << this->resolution << std::endl;
  os << "data" << std::endl;
  if (this->rootNode == nullptr) return false;
  this->rootNode->writeBinary(os);
  return true;
}

template<class KEY>
bool Octomap<KEY>::writeBinary(const std::string& filename) {
  std::ofstream binary_outfile(filename.c_str(), std::ios_base::binary);
  if (!binary_outfile.is_open()) {
    return false;
  }
  return writeBinary(binary_outfile);
}
