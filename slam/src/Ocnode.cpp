//
// Created by joao on 3/22/22.
//

#include "../include/octomap/Ocnode.h"

using namespace octomap;

Ocnode::Ocnode() {
    this->occupancy = OCCUP_UNKOWN;
}

Ocnode::~Ocnode() {
    delete this->children;
}

float Ocnode::getOccupancy() const {
    return occupancy;
}

void Ocnode::splitnode() {
    this->children = new Ocnode[8];
}
