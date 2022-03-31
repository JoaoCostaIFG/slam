//
// Created by joao on 3/23/22.
//

#ifndef SLAM_OCNODEKEY_H
#define SLAM_OCNODEKEY_H

#include <bitset>
#include <cassert>
#include <cinttypes>

#include "../parallel_hashmap/phmap_utils.h"

#include "Vector3.h"

namespace octomap {
  template<typename T = uint16_t>
  class OcNodeKey {
  private:
    T k[3];

    inline static unsigned int maxCoord = 32768;
    inline static double resolution = 0.1;
    inline static double resolution_factor = 10.0; // 1.0 / resolution

    static T coord2key(float coord) {
      return (T) floor(resolution_factor * coord) + maxCoord;
    }

    [[nodiscard]] static float key2coord(T key) {
      return float((key - maxCoord + 0.5) * resolution);
    }

  public:
    constexpr static unsigned int size = (unsigned int) sizeof(T) * 8;

    explicit OcNodeKey(const Vector3<>& p) : k{
        coord2key(p[0]),
        coord2key(p[1]),
        coord2key(p[2])
    } {}

    OcNodeKey() : OcNodeKey(Vector3()) {}

    OcNodeKey(const OcNodeKey& other) :
        k{(T) other[0], (T) other[1], (T) other[2]} {};

    [[nodiscard]] unsigned long get(unsigned int i) const {
      assert(i < 3);
      return this->k[i];
    }

    void set(unsigned int i, T val) {
      assert(i < 3);
      this->k[i] = val;
    };

    [[nodiscard]] uint8_t getStep(unsigned int i) const {
      T mask = (1 << i);
      return bool(this->get(0) & mask) * 1 +
             bool(this->get(1) & mask) * 2 +
             bool(this->get(2) & mask) * 4;
    }

    [[nodiscard]] float toCoord(unsigned int i) const {
      return OcNodeKey::key2coord(this->get(i));
    }

    [[nodiscard]] Vector3<> toCoord() const {
      return {
          this->toCoord(0),
          this->toCoord(1),
          this->toCoord(2),
      };
    }

    [[nodiscard]] unsigned long hash() const {
      return phmap::HashState::combine(0, this->get(0), this->get(1), this->get(2));
    }

    const T& operator[](unsigned int i) const {
      assert(i < 3);
      return k[i];
    }

    T& operator[](unsigned int i) {
      assert(i < 3);
      return k[i];
    }

    bool operator==(const OcNodeKey& rhs) const {
      return this->get(0) == (T) rhs[0] &&
             this->get(1) == (T) rhs[1] &&
             this->get(2) == (T) rhs[2];
    }

    bool operator!=(const OcNodeKey& rhs) const {
      return !(rhs == *this);
    }

    friend std::ostream& operator<<(std::ostream& out, OcNodeKey const& key) {
      constexpr unsigned int bitCnt = size;
      return out << "(" << std::bitset<bitCnt>(key[0]) <<
                 " " << std::bitset<bitCnt>(key[1]) <<
                 " " << std::bitset<bitCnt>(key[2]) << ")";
    }

    static void setMaxCoord(unsigned int mc) {
      maxCoord = mc;
    }

    static void setResolution(double r) {
      resolution = r;
      resolution_factor = 1.0 / r;
    }

    struct Cmp {
      bool operator()(const OcNodeKey& a, const OcNodeKey& b) const {
        return a == b;
      }
    };

    struct Hash {
      unsigned long operator()(const OcNodeKey& key) const {
        return key.hash();
      }
    };
  };
}

#endif //SLAM_OCNODEKEY_H
