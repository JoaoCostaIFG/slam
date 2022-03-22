//
// Created by joao on 3/22/22.
//

#include "../include/octomap/Ocnode.h"

Ocnode::Ocnode() {
    this->occupancy = OCCUP_UNKOWN;
}

float Ocnode::getOccupancy() const {
    return occupancy;
}
