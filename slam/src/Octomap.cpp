#include <cmath>
#include <iostream>
#include <fstream>

#include "../include/octomap/Octomap.h"

using namespace octomap;

Octomap::Octomap(const unsigned int maxDepth, const float resolution) :
        maxDepth(maxDepth), resolution(resolution) {
    this->rootNode = Ocnode();
}

Octomap::Octomap() : Octomap(DFLT_MAX_DEPTH, DFLT_RESOLUTION) {
}

Ocnode *Octomap::updateNode(const Vector3 &location) {
    Ocnode *currNode = &this->rootNode;
    Vector3 currPos = Vector3(0, 0, 0);

    for (unsigned int i = 0; i < this->maxDepth; ++i) {
        if (!currNode->hasChildren()) {
            currNode->splitNode();
            this->size += 8;
        }

        int pos = 0;
        float step = (float) pow(2.0, this->maxDepth - i) * this->resolution;
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

        currNode = currNode->getChild(pos);
        currPos = nextPos;
    }

    // std::cout << currPos << std::endl;
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