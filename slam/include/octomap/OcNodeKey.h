//
// Created by joao on 3/23/22.
//

#ifndef SLAM_OCNODEKEY_H
#define SLAM_OCNODEKEY_H

#include <cassert>
#include <cinttypes>
#include "Vector3.h"

namespace octomap {
    typedef uint16_t key_type;

    class OcNodeKey {
    private:
        key_type k[3]{};

        inline static unsigned int maxCoord = 32768;
        inline static double resolution = 0.1;
        inline static double resolution_factor = 10.0; // 1.0 / resolution

        static uint16_t coord2key(float coord);

        double key2coord(unsigned int i);

    public:
        OcNodeKey(const Vector3 &p);

        OcNodeKey();

        OcNodeKey(const OcNodeKey &other);

        [[nodiscard]] uint8_t getStep(unsigned int i);

        Vector3 toCoord();

        const key_type &operator[](unsigned int i) const {
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
    };
}

#endif //SLAM_OCNODEKEY_H
