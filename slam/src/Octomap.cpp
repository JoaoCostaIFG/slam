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
    this->stepLookupTable.reserve(this->depth + 2);
    for (unsigned int i = 0; i <= this->depth; ++i) {
        // equivalent to: 2^depth * resolution
        this->stepLookupTable[i] = this->resolution * double(1 << (this->depth - i));
    }
    this->stepLookupTable[this->depth + 1] = this->resolution / 2.0;
    // tree center for calculations
    this->treeCenter = Vector3((float) (this->stepLookupTable[0] / 2.0));
}

Octomap::Octomap() : Octomap(DFLT_MAX_DEPTH, DFLT_RESOLUTION) {}


Octomap::~Octomap() {
    delete this->rootNode;
}

OcNode *Octomap::setOccupancy(const Vector3 &location, const float occ) {
    auto key = OcNodeKey(location);

    bool createdRoot = false;
    if (this->rootNode == nullptr) {
        this->rootNode = new OcNode();
        createdRoot = true;
    }

    return this->rootNode->setOccupancy(key, this->depth, occ, createdRoot);
}

OcNode *Octomap::search(const Vector3 &location) {
    if (this->rootNode == nullptr) return nullptr;

    auto key = OcNodeKey(location);
    return this->rootNode->search(key, this->depth);
}

void Octomap::rayCast(const Vector3 &orig, const Vector3 &end) {
    auto origKey = OcNodeKey(orig);
    auto endKey = OcNodeKey(end);

    // Initialization phase
    auto coord = Vector3(orig.x(), orig.y(), 0);
    auto step = Vector3(orig.x() > end.x() ? -1 : 1,
                        orig.y() > end.y() ? -1 : 1,
                        0);

    auto tMax = Vector3(); // TODO
    auto tDelta = Vector3(); // TODO

    // Incremental phase
    while (true) { // TODO
        if (tMax.x() < tMax.y()) {
            tMax[0] = tMax.x() + tDelta.x();
            coord[0] = coord.x() + step.x();
        } else {
            tMax[1] = tMax.y() + tDelta.y();
            coord[1] = coord.y() + step.y();
        }
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
