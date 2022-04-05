//
// Created by joao on 3/22/22.
//

#ifndef SLAM_VECTOR3_H
#define SLAM_VECTOR3_H

#include <ostream>
#include <cassert>
#include <cmath>

#include "../parallel_hashmap/phmap_utils.h"

namespace octomap {
  template<typename T = float>
  class Vector3 {
  private:
    T d[3];
  public:
    Vector3(T x, T y, T z) : d{x, y, z} {}

    explicit Vector3(T n) : d{n, n, n} {}

    Vector3() : Vector3(0) {}

    // copy constructor
    Vector3(const Vector3& o) : Vector3(o.x(), o.y(), o.z()) {}

    const T& operator[](unsigned int i) const {
      assert(i < 3);
      return this->d[i];
    }

    T& operator[](unsigned int i) {
      assert(i < 3);
      return this->d[i];
    }


    [[nodiscard]] T x() const {
      return (*this)[0];
    }

    [[nodiscard]] T y() const {
      return (*this)[1];
    }

    [[nodiscard]] T z() const {
      return (*this)[2];
    }

    void setX(T x) {
      this->d[0] = x;
    }

    void setY(T y) {
      this->d[1] = y;
    }

    void setZ(T z) {
      this->d[2] = z;
    }

    [[nodiscard]] bool atLeft(const Vector3& other) const {
      return this->x() <= other.x();
    }

    [[nodiscard]] bool atRight(const Vector3& other) const {
      return !this->atLeft(other);
    }

    [[nodiscard]] bool atBack(const Vector3& other) const {
      return this->z() <= other.z();
    }

    [[nodiscard]] bool atFront(const Vector3& other) const {
      return !this->atBack(other);
    }

    [[nodiscard]] bool atDown(const Vector3& other) const {
      return this->y() <= other.y();
    }

    [[nodiscard]] bool atUp(const Vector3& other) const {
      return !this->atDown(other);
    }

    [[nodiscard]] double norm() const {
      return sqrt(
          pow(this->x(), 2) +
          pow(this->y(), 2) +
          pow(this->z(), 2)
      );
    }

    void normalize() {
      T norm = this->norm();
      this->d[0] /= norm;
      this->d[1] /= norm;
      this->d[2] /= norm;
    }

    [[nodiscard]] unsigned long hash() const {
      return phmap::HashState::combine(0, this->get(0), this->get(1), this->get(2));
    }

    Vector3 operator+(const Vector3& rhs) const {
      auto ret = Vector3(*this);
      ret[0] += rhs[0];
      ret[1] += rhs[1];
      ret[2] += rhs[2];
      return ret;
    }

    Vector3 operator-(const Vector3& rhs) const {
      auto ret = Vector3(*this);
      ret[0] -= rhs[0];
      ret[1] -= rhs[1];
      ret[2] -= rhs[2];
      return ret;
    }

    Vector3 operator*(const T factor) const {
      auto ret = Vector3(*this);
      ret[0] *= factor;
      ret[1] *= factor;
      ret[2] *= factor;
      return ret;
    }

    Vector3 operator/(const T factor) const {
      return *this * (1.0 / factor);
    }

    bool operator==(const Vector3& rhs) const {
      for (int i = 0; i < 3; ++i) {
        if ((*this)[i] != rhs[i])
          return false;
      }
      return true;
    }

    bool operator!=(const Vector3& rhs) const {
      return !(rhs == *this);
    }

    friend std::ostream& operator<<(std::ostream& out, Vector3 const& v) {
      return out <<
                 "(" << v.x() << " " << v.y() << " " << v.z() << ")";
    }

    typedef T* iterator;
    typedef const T* const_iterator;

    iterator begin() {
      return std::begin(this->d);
    }

    iterator end() {
      return std::end(this->d);
    }

    [[nodiscard]] const_iterator begin() const {
      return std::begin(this->d);
    }

    [[nodiscard]] const_iterator end() const {
      return std::end(this->d);
    }

    struct Cmp {
      bool operator()(const Vector3& a, const Vector3& b) const {
        return a == b;
      }
    };

    struct Hash {
      unsigned long operator()(const Vector3& v) const {
        return v.hash();
      }
    };
  };

  using Vector3d = Vector3<double>;
  using Vector3f = Vector3<float>;
  using Vector3i = Vector3<int>;
}

#endif //SLAM_VECTOR3_H
