//
// Created by joao on 3/23/22.
//

#include "../include/octomap/OcNodeKey.h"

using namespace octomap;

OcNodeKey::OcNodeKey(const Vector3<> &p) : k{
        boost::dynamic_bitset<>(OcNodeKey::bitCnt, (unsigned int) p[0]),
        boost::dynamic_bitset<>(OcNodeKey::bitCnt, (unsigned int) p[1]),
        boost::dynamic_bitset<>(OcNodeKey::bitCnt, (unsigned int) p[2]),
} {}

OcNodeKey::OcNodeKey() : OcNodeKey(Vector3()) {}

uint8_t OcNodeKey::getStep(unsigned int i) const {
    assert(i <= 15);
    uint16_t mask = (1 << i);
    return bool((*this)[0] & mask) * 1 +
           bool((*this)[1] & mask) * 2 +
           bool((*this)[2] & mask) * 4;
}

float OcNodeKey::toCoord(unsigned int i) const {
    return OcNodeKey::key2coord(this->k[i]);
}

Vector3<> OcNodeKey::toCoord() const {
    return {
            OcNodeKey::key2coord(this->k[0]),
            OcNodeKey::key2coord(this->k[1]),
            OcNodeKey::key2coord(this->k[2])
    };
}
