//
// Created by joao on 3/23/22.
//

#ifndef SLAM_OCNODEKEY_HXX
#define SLAM_OCNODEKEY_HXX

#include <bitset>
#include <boost/dynamic_bitset.hpp>
#include <cassert>
#include <cinttypes>
#include <memory>
#include <unordered_set>

#include "Vector3.h"

namespace octomap {
  class OcNodeKey {
  protected:
    inline static unsigned int maxCoord = 32768;
    inline static double resolution = 0.1;
    inline static double resolution_factor = 10.0; // 1.0 / resolution

  public:
    [[nodiscard]] virtual uint8_t getStep(unsigned int i) const = 0;

    [[nodiscard]] virtual float toCoord(unsigned int i) const = 0;

    [[nodiscard]] virtual Vector3<> toCoord() const = 0;

    [[nodiscard]] virtual unsigned long get(unsigned int i) const = 0;

    virtual void set(unsigned int i, unsigned int val) = 0;

    bool operator==(const OcNodeKey& rhs) const {
      return this->get(0) == rhs.get(0) &&
             this->get(1) == rhs.get(1) &&
             this->get(2) == rhs.get(2);
    }

    bool operator!=(const OcNodeKey& rhs) const {
      return !(rhs == *this);
    }

    static void setMaxCoord(unsigned int mc) {
      OcNodeKey::maxCoord = mc;
    }

    static void setResolution(double r) {
      OcNodeKey::resolution = r;
      OcNodeKey::resolution_factor = 1.0 / r;
    }

    struct Cmp {
      bool operator()(const OcNodeKey& a, const OcNodeKey& b) const {
        return a == b;
      }

      bool operator()(const std::unique_ptr<OcNodeKey>& a, const std::unique_ptr<OcNodeKey>& b) const {
        return this->operator()(*a, *b);
      }
    };

    struct Hash {
      size_t operator()(const OcNodeKey& key) const {
        return (size_t) key.get(0) +
               2287 * (size_t) key.get(1) +
               104729 * (size_t) key.get(2);
      }

      size_t operator()(const std::unique_ptr<OcNodeKey>& key) const {
        return this->operator()(*key);
      }
    };
  };

  typedef std::unique_ptr<OcNodeKey> OcNodeKeyPtr;
  typedef std::unordered_set<OcNodeKeyPtr, OcNodeKey::Hash, OcNodeKey::Cmp> KeySet;

  template<typename T = uint16_t>
  class OcNodeKeyInt : public OcNodeKey {
  private:
    T k[3];

    static T coord2key(float coord) {
      return (T) floor(resolution_factor * coord) + maxCoord;
    }

    [[nodiscard]] static float key2coord(T key) {
      return float((key - maxCoord + 0.5) * resolution);
    }

  public:
    explicit OcNodeKeyInt(const Vector3<>& p) : k{
        coord2key(p[0]),
        coord2key(p[1]),
        coord2key(p[2])
    } {}

    OcNodeKeyInt() : OcNodeKeyInt(Vector3()) {}

    explicit OcNodeKeyInt(const OcNodeKey& other) :
        k{(T) other.get(0), (T) other.get(1), (T) other.get(2)} {};

    ~OcNodeKeyInt() = default;

    [[nodiscard]] uint8_t getStep(unsigned int i) const override {
      T mask = (1 << i);
      return bool((*this)[0] & mask) * 1 +
             bool((*this)[1] & mask) * 2 +
             bool((*this)[2] & mask) * 4;
    }

    [[nodiscard]] float toCoord(unsigned int i) const override {
      return OcNodeKeyInt::key2coord((*this)[i]);
    }

    [[nodiscard]] Vector3<> toCoord() const override {
      return {
          this->toCoord(0),
          this->toCoord(1),
          this->toCoord(2),
      };
    }

    [[nodiscard]] unsigned long get(unsigned int i) const override {
      assert(i < 3);
      return this->k[i];
    }

    void set(unsigned int i, unsigned int val) override {
      assert(i < 3);
      this->k[i] = val;
    };

    const T& operator[](unsigned int i) const {
      assert(i < 3);
      return k[i];
    }

    T& operator[](unsigned int i) {
      assert(i < 3);
      return k[i];
    }

    friend std::ostream& operator<<(std::ostream& out, OcNodeKeyInt const& key) {
      // TODO should adapt
      return out << "(" << std::bitset<16>(key[0]) <<
                 " " << std::bitset<16>(key[1]) <<
                 " " << std::bitset<16>(key[2]) << ")";
    }
  };

  class OcNodeKeyBitSet : public OcNodeKey {
  private:
    boost::dynamic_bitset<> k[3];
    unsigned int bitCnt = 16;

    static unsigned long coord2key(float coord) {
      return (unsigned long) (floor(resolution_factor * coord) + maxCoord);
    }

    [[nodiscard]] static float key2coord(const boost::dynamic_bitset<>& key) {
      return float((key.to_ulong() - maxCoord + 0.5) * resolution);
    }

  public:
    explicit OcNodeKeyBitSet(unsigned int bitCnt, const Vector3<>& p) : k{
        boost::dynamic_bitset<>(bitCnt, coord2key(p[0])),
        boost::dynamic_bitset<>(bitCnt, coord2key(p[1])),
        boost::dynamic_bitset<>(bitCnt, coord2key(p[2])),
    }, bitCnt(bitCnt) {}

    explicit OcNodeKeyBitSet(unsigned int bitCnt) : OcNodeKeyBitSet(bitCnt, Vector3()) {}

    OcNodeKeyBitSet(unsigned int bitCnt, const OcNodeKey& o) :
        k{
            boost::dynamic_bitset<>(bitCnt, o.get(0)),
            boost::dynamic_bitset<>(bitCnt, o.get(1)),
            boost::dynamic_bitset<>(bitCnt, o.get(2)),
        }, bitCnt(bitCnt) {}

    ~OcNodeKeyBitSet() = default;

    [[nodiscard]] uint8_t getStep(unsigned int i) const override {
      unsigned long mask = (1 << i);
      return bool(this->get(0) & mask) * 1 +
             bool(this->get(1) & mask) * 2 +
             bool(this->get(2) & mask) * 4;
    }

    [[nodiscard]] float toCoord(unsigned int i) const override {
      return key2coord(this->k[i]);
    }

    [[nodiscard]] Vector3<> toCoord() const override {
      return {
          key2coord(this->k[0]),
          key2coord(this->k[1]),
          key2coord(this->k[2])
      };
    }

    [[nodiscard]] unsigned long get(unsigned int i) const override {
      return this->k[i].to_ulong();
    }

    void set(unsigned int i, unsigned int val) override {
      assert(i < 3);
      this->k[i] = boost::dynamic_bitset(bitCnt, val);
    }

    unsigned long operator[](unsigned int i) const {
      assert(i < 3);
      return this->get(i);
    }

    friend std::ostream& operator<<(std::ostream& out, OcNodeKeyBitSet const& key) {
      return out << "(" << key[0] <<
                 " " << key[1] <<
                 " " << key[2] << ")";
    }
  };

  inline std::unique_ptr<OcNodeKey> newOcNodeKey(unsigned int maxDepth, const Vector3f& initializer = Vector3()) {
    if (maxDepth <= 8) {
      return std::make_unique<OcNodeKeyInt<uint8_t>>(initializer);
    } else if (maxDepth <= 16) {
      return std::make_unique<OcNodeKeyInt<uint16_t>>(initializer);
    } else if (maxDepth <= 32) {
      return std::make_unique<OcNodeKeyInt<uint32_t>>(initializer);
    } else if (maxDepth <= 64) {
      return std::make_unique<OcNodeKeyInt<uint64_t>>(initializer);
    } else {
      return std::make_unique<OcNodeKeyBitSet>(maxDepth, initializer);
    }
  }

  inline std::unique_ptr<OcNodeKey> newOcNodeKey(unsigned int maxDepth, const OcNodeKey& other) {
    if (maxDepth <= 8) {
      return std::make_unique<OcNodeKeyInt<uint8_t>>(other);
    } else if (maxDepth <= 16) {
      return std::make_unique<OcNodeKeyInt<uint16_t>>(other);
    } else if (maxDepth <= 32) {
      return std::make_unique<OcNodeKeyInt<uint32_t>>(other);
    } else if (maxDepth <= 64) {
      return std::make_unique<OcNodeKeyInt<uint64_t>>(other);
    } else {
      return std::make_unique<OcNodeKeyBitSet>(maxDepth, other);
    }
  }
}

#endif //SLAM_OCNODEKEY_HXX
