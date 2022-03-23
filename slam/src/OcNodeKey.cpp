//
// Created by joao on 3/23/22.
//

#include <cmath>

#include "../include/octomap/OcNodeKey.h"

using namespace octomap;

OcNodeKey::OcNodeKey(const Vector3 &p) {
    k[0] = coord2key(p[0]);
    k[1] = coord2key(p[1]);
    k[2] = coord2key(p[2]);
}

OcNodeKey::OcNodeKey() : OcNodeKey(Vector3()) {}

OcNodeKey::OcNodeKey(const octomap::OcNodeKey &other) :
        k{other[0], other[1], other[2]} {}

uint16_t OcNodeKey::coord2key(float coord) {
    return (uint16_t)((int) floor(resolution_factor * coord) + maxCoord);
}

double OcNodeKey::key2coord(unsigned int i) {
    return (double((int) (*this)[i] - (int) OcNodeKey::maxCoord) + 0.5) * OcNodeKey::resolution;
}

uint8_t OcNodeKey::getStep(unsigned int i) {
    assert(i < 15);
    uint16_t mask = (1 << i);
    return ((*this)[0] & mask) * 1 + ((*this)[1] & mask) * 2 + ((*this)[2] & mask) * 4;
}

Vector3 OcNodeKey::toCoord() {
    return {
            static_cast<float>(this->key2coord(0)),
            static_cast<float>(this->key2coord(1)),
            static_cast<float>(this->key2coord(2))
    };
}
