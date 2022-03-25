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

uint8_t OcNodeKey::getStep(unsigned int i) const {
    assert(i <= 15);
    uint16_t mask = (1 << i);
    return bool((*this)[0] & mask) * 1 + bool((*this)[1] & mask) * 2 + bool((*this)[2] & mask) * 4;
}

float OcNodeKey::toCoord(unsigned int i) const {
    return OcNodeKey::key2coord((*this)[i]);
}

Vector3 OcNodeKey::toCoord() const {
    return {
            OcNodeKey::key2coord((*this)[0]),
            OcNodeKey::key2coord((*this)[1]),
            OcNodeKey::key2coord((*this)[2])
    };
}
