#include <iostream>
#include <fstream>

#include "../include/octomap/Octomap.h"
#include "../include/octomap/OcNodeKey.h"

using namespace octomap;

Octomap::Octomap(const unsigned int maxDepth, const double resolution) :
        depth(maxDepth), resolution(resolution) {
    OcNodeKey::setMaxCoord((int) pow(2, maxDepth - 1));
    OcNodeKey::setResolution(resolution);

    this->rootNode = OcNode();

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

OcNode *Octomap::updateNode(const Vector3 &location) {
    OcNode *currNode = &this->rootNode;
    auto currPos = Vector3();


    for (unsigned int i = 0; i < this->depth; ++i) {
        int pos = 0;
        auto step = (float) (this->stepLookupTable[i + 1]); // query 1 ahead = divide by 2
        auto nextPos = Vector3(currPos);
        if (currPos.atLeft(location)) {
            pos += RIGHT;
            nextPos.setX(nextPos.x() + step);
        } else {
            nextPos.setX(nextPos.x() - step);
        }
        if (currPos.atBack(location)) {
            pos += FRONT;
            nextPos.setZ(nextPos.z() + step);
        } else {
            nextPos.setZ(nextPos.z() - step);
        }
        if (currPos.atDown(location)) {
            pos += UP;
            nextPos.setY(nextPos.y() + step);
        } else {
            nextPos.setY(nextPos.y() - step);
        }

        if (!currNode->childExists(pos)) {
            ++this->size;
            currNode->createChild(pos);
        }
        currNode = currNode->getChild(pos);
        currPos = nextPos;
        std::cout << currPos << std::endl;
    }

    return currNode;
}

bool Octomap::writeBinary(std::ostream &os) {
    os << "# Octomap OcTree binary file\n";
    os << "id OcTree\n";
    os << "size " << this->size << std::endl;
    os << "res " << this->resolution << std::endl;
    os << "data" << std::endl;
    this->rootNode.writeBinary(os);

    return true;
}

bool Octomap::writeBinary(const std::string &filename) {
    std::ofstream binary_outfile(filename.c_str(), std::ios_base::binary);
    if (!binary_outfile.is_open()) {
        return false;
    }
    return writeBinary(binary_outfile);
}