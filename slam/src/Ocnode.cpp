//
// Created by joao on 3/22/22.
//

#include <iostream>
#include "../include/octomap/Ocnode.h"

using namespace octomap;

Ocnode::Ocnode() {
    this->occupancy = OCCUP_UNKOWN;
}

Ocnode::~Ocnode() {
    if (this->children != nullptr) {
        for (int i = 0; i < 8; ++i) {
            delete this->children[i];
        }
    }
    delete[] this->children;
}

Ocnode *Ocnode::getChild(unsigned int pos) const {
    assert(pos < 8);
    if (this->children == nullptr) return nullptr;
    return this->children[pos];
}

bool Ocnode::createChild(unsigned int pos) {
    assert(pos < 8);
    if (this->children == nullptr) {
        this->expandNode();
    }

    if (this->childExists(pos)) return false;
    this->children[pos] = new Ocnode();
    return true;
}

void Ocnode::expandNode() {
    this->children = new Ocnode *[8]{nullptr};
}

bool Ocnode::hasChildren() const {
    if (this->children == nullptr) return false;
    for (int i = 0; i < 8; ++i) {
        if (this->children[i] != nullptr) return true;
    }
    return false;
}

bool Ocnode::childExists(unsigned int i) const {
    if (this->children == nullptr) return false;
    return this->children[i] != nullptr;
}

void Ocnode::writeBinaryInner(std::ostream &os, int baseI, std::bitset<8> &childBitset) const {
    // 00 : child is unknown node
    // 01 : child is occupied node
    // 10 : child is free node
    // 11 : child has children

    for (unsigned int i = 0; i < 4; ++i) {
        if (this->childExists(baseI + i)) {
            const Ocnode *child = this->getChild(baseI + i);
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

void Ocnode::writeBinary(std::ostream &os) const {
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
            const Ocnode *child = this->getChild(i);
            if (child->hasChildren()) child->writeBinary(os);
        }
    }
}
