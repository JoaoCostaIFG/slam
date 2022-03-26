//
// Created by joao on 3/23/22.
//

#ifndef SLAM_OCNODEKEY_H
#define SLAM_OCNODEKEY_H

#include <cassert>
#include <bitset>
#include <cinttypes>
#include <memory>

#include "Vector3.h"

namespace octomap {
    class OcNodeKey {
    protected:
        inline static unsigned int maxCoord = 32768;
        inline static double resolution = 0.1;
        inline static double resolution_factor = 10.0; // 1.0 / resolution

    public:
        virtual ~OcNodeKey() = 0;

        [[nodiscard]] virtual uint8_t getStep(unsigned int i) const = 0;

        [[nodiscard]] virtual float toCoord(unsigned int i) const = 0;

        [[nodiscard]] virtual Vector3<> toCoord() const = 0;

        static void setMaxCoord(unsigned int mc) {
            OcNodeKey::maxCoord = mc;
        }

        static void setResolution(double r) {
            OcNodeKey::resolution = r;
            OcNodeKey::resolution_factor = 1.0 / r;
        }
    };

    template<typename T = uint16_t>
    class OcNodeKeyInt : public OcNodeKey {
    private:
        T k[3];

        static T coord2key(float coord) {
            return (T) ((int) floor(resolution_factor * coord) + maxCoord);
        }

        [[nodiscard]] static float key2coord(T key) {
            return float((key - maxCoord + 0.5) * resolution);
        }

    public:
        explicit OcNodeKeyInt(const Vector3<> &p) : k{
                coord2key(p[0]),
                coord2key(p[1]),
                coord2key(p[2])
        } {}

        OcNodeKeyInt() : OcNodeKeyInt(Vector3()) {}

        OcNodeKeyInt(const OcNodeKeyInt &other) : k{other[0], other[1], other[2]} {};

        [[nodiscard]] uint8_t getStep(unsigned int i) const {
            T mask = (1 << i);
            return bool((*this)[0] & mask) * 1 +
                   bool((*this)[1] & mask) * 2 +
                   bool((*this)[2] & mask) * 4;
        }

        [[nodiscard]] float toCoord(unsigned int i) const {
            return OcNodeKeyInt::key2coord((*this)[i]);
        }

        [[nodiscard]] Vector3<> toCoord() const {
            return {
                    this->toCoord(0),
                    this->toCoord(1),
                    this->toCoord(2),
            };
        }

        const T &operator[](unsigned int i) const {
            assert(i < 3);
            return k[i];
        }

        T &operator[](unsigned int i) {
            assert(i < 3);
            return k[i];
        }

        bool operator==(const OcNodeKeyInt &rhs) const {
            return (*this)[0] == rhs[0] &&
                   (*this)[1] == rhs[1] &&
                   (*this)[2] == rhs[2];
        }

        bool operator!=(const OcNodeKeyInt &rhs) const {
            return !(rhs == *this);
        }

        friend std::ostream &operator<<(std::ostream &out, OcNodeKeyInt const &key) {
            // TODO should adapt
            return out << "(" << std::bitset<16>(key[0]) <<
                       " " << std::bitset<16>(key[1]) <<
                       " " << std::bitset<16>(key[2]) << ")";
        }
    };

    std::unique_ptr<OcNodeKey> newOcNodeKey(unsigned int maxDepth) {
        if (maxDepth <= 8) {
            return std::make_unique<OcNodeKeyInt<uint8_t>>();
        } else if (maxDepth <= 16) {
            return std::make_unique<OcNodeKeyInt<uint16_t>>();
        } else if (maxDepth <= 32) {
            return std::make_unique<OcNodeKeyInt<uint32_t>>();
        } else if (maxDepth <= 64) {
            return std::make_unique<OcNodeKeyInt<uint64_t>>();
        } else {

        }
    }
}

#endif //SLAM_OCNODEKEY_H
