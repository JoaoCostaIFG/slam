#include <cassert>
#include <cmath>
#include <iostream>
#include <fstream>

#ifdef _OPENMP

#include <omp.h>

#endif

#include "../include/octomap/Octomap.h"

using namespace octomap;

Octomap::Octomap(const unsigned int maxDepth, const double resolution) :
    depth(maxDepth), resolution(resolution) {
  assert(this->depth >= 1);
  OcNodeKey::setMaxCoord((int) pow(2, maxDepth - 1));
  OcNodeKey::setResolution(resolution);

  this->rootNode = nullptr;

  // pre-calculate step sizes
  this->stepLookupTable.reserve(this->depth + 2);
  for (unsigned int i = 0; i <= this->depth; ++i) {
    // equivalent to: 2^depth * resolution
    this->stepLookupTable[i] = this->resolution * double(1 << (this->depth - i));
  }
  this->stepLookupTable[this->depth + 1] = this->resolution / 2.0;
  // tree center for calculations
  //this->treeCenter = Vector3((float) (this->stepLookupTable[0] / 2.0));
}

Octomap::Octomap() : Octomap(DFLT_MAX_DEPTH, DFLT_RESOLUTION) {}


Octomap::~Octomap() {
  delete this->rootNode;
}

bool Octomap::createRootIfNeeded() {
  if (this->rootNode == nullptr) {
    this->rootNode = new OcNode();
    return true;
  }
  return false;
}

OcNode *Octomap::setOccupancy(const OcNodeKey &key, float occ) {
  bool createdRoot = this->createRootIfNeeded();
  return this->rootNode->setOccupancy(key, this->depth, occ, createdRoot);
}

OcNode *Octomap::setOccupancy(const Vector3<> &location, const float occ) {
  return this->setOccupancy(*newOcNodeKey(this->depth, location), occ);
}

OcNode *Octomap::updateLogOdds(const OcNodeKey &key, const float logOdds) {
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

OcNode *Octomap::updateOccupancy(const OcNodeKey &key, const float occ) {
  return this->updateLogOdds(key, (float) OcNode::prob2logodds(occ));
}

OcNode *Octomap::updateOccupancy(const Vector3<> &location, const float occ) {
  return this->updateOccupancy(*newOcNodeKey(this->depth, location), occ);
}

OcNode *Octomap::search(const OcNodeKey &key) {
  if (this->rootNode == nullptr) return nullptr;
  return this->rootNode->search(key, this->depth);
}

OcNode *Octomap::search(const Vector3<> &location) {
  if (this->rootNode == nullptr) return nullptr;
  return this->search(*newOcNodeKey(this->depth, location));
}

std::vector<std::unique_ptr<OcNodeKey>> Octomap::rayCast(const Vector3<> &orig, const Vector3<> &end) {
  auto ray = std::vector<std::unique_ptr<OcNodeKey>>();

  auto coord = newOcNodeKey(this->depth, orig);
  auto endKey = newOcNodeKey(this->depth, end);
  if (coord == endKey) return ray;

  // Initialization phase
  auto step = Vector3i();
  auto tMax = Vector3d();
  auto tDelta = Vector3d();

  auto direction = (end - orig);
  direction.normalize();

  for (int i = 0; i < 3; ++i) {
    if (direction[i] > 0) step[i] = 1.0;
    else step[i] = -1.0;

    double voxelBorder = coord->toCoord(i) +
                         step[i] * this->stepLookupTable[this->depth + 1];

    tMax[i] = (voxelBorder - orig[i]) / direction[i];
    tDelta[i] = this->resolution / fabs(direction[i]);
  }

  // Incremental phase
  Vector3 origCord = coord->toCoord();
  double length = (endKey->toCoord() - origCord).norm();
  double *min;
  while (*coord != *endKey &&
         (
             *(min = std::min_element(tMax.begin(), tMax.end())) <= length ||
             (coord->toCoord() - origCord).norm() <= length
         )) {
    //std::cout << coord->get(0) << " " << coord->get(1) << " " << coord->get(2) <<
    //          " " << coord->toCoord()
    //          << std::endl;
    auto newCoord = newOcNodeKey(this->depth, *coord);
    ray.push_back(std::move(coord));
    int coordInd = int(min - std::begin(tMax));

    tMax[coordInd] += tDelta[coordInd];
    coord = std::move(newCoord);
    coord->set(coordInd, coord->get(coordInd) + step[coordInd]);
  }

  return ray;
}

OcNode *Octomap::rayCastUpdate(const Vector3<> &orig, const Vector3<> &end, float occ) {
  auto ray = this->rayCast(orig, end);
  for (auto &it: ray)
    this->setEmpty(*it);
  return this->updateOccupancy(end, occ);
}

void Octomap::pointcloudUpdate(const std::vector<Vector3f> &pointcloud, const Vector3f &origin) {
  KeySet freeNodes, occupiedNodes;

#ifdef _OPENMP
#pragma omp parallel for schedule(guided) default(none) shared(pointcloud, origin, freeNodes, occupiedNodes)
#endif
  for (const auto &endpoint: pointcloud) {
    auto ray = this->rayCast(origin, endpoint);
#ifdef _OPENMP
#pragma omp critical (freeNodes_insert)
#endif
    {
      for (auto &rayPoint: ray) {
        freeNodes.insert(std::move(rayPoint));
      }
    }
#ifdef _OPENMP
#pragma omp critical (occupiedNodes_insert)
#endif
    {
      occupiedNodes.insert(newOcNodeKey(this->depth, endpoint));
    }
  }

  // remove updates on freenodes that will be set as occupied
  for (auto it = freeNodes.begin(); it != freeNodes.end();) {
    if (occupiedNodes.find(*it) != occupiedNodes.end()) {
      it = freeNodes.erase(it);
    } else {
      ++it;
    }
  }

  // update nodes
  for (auto &endpoint: freeNodes) {
    this->updateOccupancy(*endpoint, 0);
  }
  for (auto &endpoint: occupiedNodes) {
    this->updateOccupancy(*endpoint, 1);
  }
}

bool Octomap::writeBinary(std::ostream &os) {
  os << "# Octomap OcTree binary file\n";
  os << "id OcTree\n";
  os << "size " << this->getSize() << std::endl;
  os << "res " << this->resolution << std::endl;
  os << "data" << std::endl;
  if (this->rootNode == nullptr) return false;
  this->rootNode->writeBinary(os);
  return true;
}

bool Octomap::writeBinary(const std::string &filename) {
  std::ofstream binary_outfile(filename.c_str(), std::ios_base::binary);
  if (!binary_outfile.is_open()) {
    return false;
  }
  return writeBinary(binary_outfile);
}
