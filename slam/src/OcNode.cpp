//
// Created by joao on 3/22/22.
//

#include <iostream>
#include "../include/octomap/OcNode.h"

using namespace octomap;


OcNode::OcNode(float occ) : occupancy(occ) {}

OcNode::OcNode() : OcNode(OCCUP_UNKOWN) {}

OcNode::~OcNode() {
    if (this->children != nullptr) {
        for (int i = 0; i < 8; ++i) {
            delete this->children[i];
        }
    }
    delete[] this->children;
}

OcNode *OcNode::getChild(unsigned int pos) const {
    assert(pos < 8);
    if (this->children == nullptr) return nullptr;
    return this->children[pos];
}

OcNode *OcNode::createChild(unsigned int pos) {
    assert(pos < 8);
    if (this->children == nullptr) {
        this->allocChildren();
    }

    if (!this->childExists(pos)) this->children[pos] = new OcNode();
    return this->children[pos];
}

void OcNode::allocChildren() {
    this->children = new OcNode *[8]{nullptr};
}

void OcNode::expandNode() {
    assert(!this->hasChildren());
    if (this->children == nullptr) {
        this->allocChildren();
    }

    for (int i = 0; i < 8; ++i) {
        this->children[i] = new OcNode(this->occupancy);
    }
}

bool OcNode::hasChildren() const {
    if (this->children == nullptr) return false;
    for (int i = 0; i < 8; ++i) {
        if (this->children[i] != nullptr) return true;
    }
    return false;
}

bool OcNode::childExists(unsigned int i) const {
    if (this->children == nullptr) return false;
    return this->children[i] != nullptr;
}

// Use the max of the children's occupancy
void OcNode::updateOccBasedOnChildren() {
    if (this->children == nullptr) return;
    float max = std::numeric_limits<float>::min();
    for (int i = 0; i < 8; ++i) {
        OcNode *child = this->children[i];
        if (child == nullptr) continue;
        if (child->getOccupancy() > max) max = child->getOccupancy();
    }
    this->occupancy = max;
}

OcNode *OcNode::setOccupancy(const OcNodeKey &key, const unsigned int depth, const float occ, const bool justCreated) {
    unsigned int d = depth - 1;
    bool createdChild = false;
    OcNode *child;

    // follow down to last level
    if (d > 0) { // depth - 1 > 0
        unsigned int pos = key.getStep(d);
        std::cout << pos << std::endl;
        if (!this->childExists(pos)) {
            // child does not exist, but maybe it's a pruned node?
            if (!this->hasChildren() && !justCreated) {
                // current node does not have children AND it is not a new node
                // -> expand pruned node
                this->expandNode();
                child = this->getChild(pos);
            } else {
                // not a pruned node, create requested child
                child = this->createChild(pos);
                createdChild = true;
            }
        }

        child->setOccupancy(key, d, occ, createdChild);

        // prune node if possible, otherwise set own probability
        // note: combining both did not lead to a speedup!
        //if (this->pruneNode(node)) {
        //    // return pointer to current parent (pruned), the just updated node no longer exists
        //    retval = node;
        //} else {
        //    node->updateOccupancyChildren();
        //}

        this->updateOccBasedOnChildren();
        return child;
    } else { // at last level, update node, end of recursion
        this->setOccupancy(occ);
        return this;
    }
}

void OcNode::writeBinaryInner(std::ostream &os, int baseI, std::bitset<8> &childBitset) const {
    // 00 : child is unknown node
    // 01 : child is occupied node
    // 10 : child is free node
    // 11 : child has children

    for (unsigned int i = 0; i < 4; ++i) {
        if (this->childExists(baseI + i)) {
            const OcNode *child = this->getChild(baseI + i);
            if (child->hasChildren()) {
                // 11 : child has children
                childBitset[i * 2] = 1;
                childBitset[i * 2 + 1] = 1;
            } else if (child->isOccupied()) {
                // 01 : child is occupied node
                childBitset[i * 2] = 0;
                childBitset[i * 2 + 1] = 1;
            } else {
                // 10 : child is free node
                childBitset[i * 2] = 1;
                childBitset[i * 2 + 1] = 0;
            }
        } else {
            // 00 : child is unknown node
            childBitset[i * 2] = 0;
            childBitset[i * 2 + 1] = 0;
        }
    }
}

void OcNode::writeBinary(std::ostream &os) const {
    std::bitset<8> child1to4;
    std::bitset<8> child5to8;

    this->writeBinaryInner(os, 0, child1to4);
    this->writeBinaryInner(os, 4, child5to8);

    char child1to4_char = (char) child1to4.to_ulong();
    char child5to8_char = (char) child5to8.to_ulong();

    os.write((char *) &child1to4_char, sizeof(char));
    os.write((char *) &child5to8_char, sizeof(char));

    // write children
    for (unsigned int i = 0; i < 8; i++) {
        if (this->childExists(i)) {
            const OcNode *child = this->getChild(i);
            if (child->hasChildren()) child->writeBinary(os);
        }
    }
}
