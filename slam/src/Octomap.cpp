#include <iostream>
#include <fstream>

#include "../include/octomap/Octomap.h"

using namespace octomap;

Octomap::Octomap(const unsigned int maxDepth, const double resolution) :
        depth(maxDepth), resolution(resolution) {
    OcNodeKey::setMaxCoord((int) pow(2, maxDepth - 1));
    OcNodeKey::setResolution(resolution);

    this->rootNode = nullptr;

    // pre-calculate step sizes
    this->stepLookupTable.reserve(this->depth + 1);
    for (unsigned int i = 0; i <= this->depth; ++i) {
        // equivalent to: 2^depth * resolution
        this->stepLookupTable[i] = this->resolution * double(1 << (this->depth - i));
    }
    // tree center for calculations
    this->treeCenter = Vector3((float) (this->stepLookupTable[0] / 2.0));
}

Octomap::Octomap() : Octomap(DFLT_MAX_DEPTH, DFLT_RESOLUTION) {}

OcNode *Octomap::setOccupancy(const Vector3 &location, const float occ) {
    auto key = OcNodeKey(location);
    bool createdRoot = false;
    if (this->rootNode == nullptr) {
        this->rootNode = new OcNode();
        createdRoot = true;
    }
    return this->rootNode->setOccupancy(key, this->depth, occ, createdRoot);
}

bool Octomap::writeBinary(std::ostream &os) {
    os << "# Octomap OcTree binary file\n";
    os << "id OcTree\n";
    os << "size " << this->size << std::endl;
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