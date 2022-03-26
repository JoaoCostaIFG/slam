#include <cassert>
#include <cmath>
#include <iostream>
#include <fstream>

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

OcNode *Octomap::setOccupancy(const Vector3<> &location, const float occ) {
    auto key = OcNodeKey(location);

    bool createdRoot = false;
    if (this->rootNode == nullptr) {
        this->rootNode = new OcNode();
        createdRoot = true;
    }

    return this->rootNode->setOccupancy(key, this->depth, occ, createdRoot);
}

OcNode *Octomap::search(const Vector3<> &location) {
    if (this->rootNode == nullptr) return nullptr;

    auto key = OcNodeKey(location);
    return this->rootNode->search(key, this->depth);
}

std::vector<OcNodeKey> Octomap::rayCast(const Vector3<> &orig, const Vector3<> &end) {
    std::vector ray = std::vector<OcNodeKey>();

    auto origKey = OcNodeKey(orig);
    auto endKey = OcNodeKey(end);
    if (origKey == endKey) return ray;
    ray.push_back(origKey);

    // Initialization phase
    auto coord = origKey;
    auto step = Vector3<int>();
    auto tMax = Vector3<double>();
    auto tDelta = Vector3<double>();

    auto direction = (end - orig);
    direction.normalize();

    for (int i = 0; i < 3; ++i) {
        if (direction[i] > 0) step[i] = 1.0;
        else step[i] = -1.0;

        double voxelBorder = coord.toCoord(i) +
                             step[i] * this->stepLookupTable[this->depth + 1];

        tMax[i] = (voxelBorder - orig[i]) / direction[i];
        tDelta[i] = this->resolution / fabs(direction[i]);
    }

    // Incremental phase
    while (true) {
        auto it = std::min_element(std::begin(tMax), std::end(tMax));
        int coordInd = int(it - std::begin(tMax));

        tMax[coordInd] += tDelta[coordInd];
        coord[coordInd] += step[coordInd];
        if (coord == endKey) break;
        ray.push_back(coord);
    }

    return ray;
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
