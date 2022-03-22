//
// Created by joao on 3/22/22.
//

#include "../include/octomap/Ocnode.h"

using namespace octomap;

Ocnode::Ocnode() {
    this->occupancy = OCCUP_UNKOWN;
}

Ocnode::~Ocnode() {
    // TODO :)
    //delete this->children;
}

Ocnode *Ocnode::getChild(unsigned int pos) {
    assert(pos < 8);
    if (this->children == nullptr) return nullptr;
    return &this->children[pos];
}

float Ocnode::getOccupancy() const {
    return occupancy;
}

void Ocnode::splitNode() {
    this->children = new Ocnode[8];
}

bool Ocnode::hasChildren() {
    if (this->children == nullptr) return false;
    // TODO is there an interest to not alloc all children on node division
    /*
    for (int i = 0; i < 8; ++i) {
        if (this->children[i] != nullptr) return true;
    }
    */
    return true;
}
