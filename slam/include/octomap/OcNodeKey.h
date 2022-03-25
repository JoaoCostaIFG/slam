//
// Created by joao on 3/23/22.
//

#ifndef SLAM_OCNODEKEY_H
#define SLAM_OCNODEKEY_H

#include <cassert>
#include <cinttypes>
#include <bitset>

#include "Vector3.h"

namespace octomap {
    typedef uint16_t key_type;

    class OcNodeKey {
    private:
        key_type k[3]{};

        inline static unsigned int maxCoord = 32768;
        inline static double resolution = 0.1;
        inline static double resolution_factor = 10.0; // 1.0 / resolution

        static uint16_t coord2key(float coord) {
            return (uint16_t) ((int) floor(resolution_factor * coord) + maxCoord);
        }

        [[nodiscard]] static float key2coord(uint16_t key) {
            return float((key - maxCoord + 0.5) * resolution);
        }

    public:
        explicit OcNodeKey(const Vector3 &p);

        OcNodeKey();

        OcNodeKey(const OcNodeKey &other);

        [[nodiscard]] uint8_t getStep(unsigned int i) const;

        [[nodiscard]] float toCoord(unsigned int i) const;

        [[nodiscard]] Vector3 toCoord() const;

        const key_type &operator[](unsigned int i) const {
            assert(i < 3);
            return k[i];
        }

        key_type &operator[](unsigned int i) {
            assert(i < 3);
            return k[i];
        }

        bool operator==(const OcNodeKey &rhs) const {
            return (*this)[0] == rhs[0] && (*this)[1] == rhs[1] && (*this)[2] == rhs[2];
        }

        bool operator!=(const OcNodeKey &rhs) const {
            return !(rhs == *this);
        }

        static void setMaxCoord(unsigned int mc) {
            OcNodeKey::maxCoord = mc;
        }

        static void setResolution(double r) {
            OcNodeKey::resolution = r;
            OcNodeKey::resolution_factor = 1.0 / r;
        }

        friend std::ostream &operator<<(std::ostream &out, OcNodeKey const &key) {
            return out << "(" << std::bitset<16>(key[0]) <<
                       " " << std::bitset<16>(key[1]) <<
                       " " << std::bitset<16>(key[2]) << ")";
        }
    };
}

#endif //SLAM_OCNODEKEY_H
