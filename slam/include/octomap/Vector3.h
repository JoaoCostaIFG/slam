#ifndef SLAM_VECTOR3_H
#define SLAM_VECTOR3_H

#include <ostream>
#include <cassert>
#include <cmath>

#include "../parallel_hashmap/phmap_utils.h"

namespace octomap {
template<typename T = double>
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

  /**
   * Get the first component of this vector (x-dimension).
   * @return The value of the first component of the vector.
   */
  [[nodiscard]] T x() const {
    return (*this)[0];
  }

  /**
   * Get the second component of this vector (y-dimension).
   * @return The value of the second component of the vector.
   */
  [[nodiscard]] T y() const {
    return (*this)[1];
  }

  /**
   * Get the third component of this vector (z-dimension).
   * @return The value of the third component of the vector.
   */
  [[nodiscard]] T z() const {
    return (*this)[2];
  }

  /**
   * hash_table the value of the first component of the vector (x-dimension).
   * @param x The value to set.
   */
  void setX(T x) {
    this->d[0] = x;
  }

  /**
   * hash_table the value of the second component of the vector (y-dimension).
   * @param y The value to set.
   */
  void setY(T y) {
    this->d[1] = y;
  }

  /**
   * hash_table the value of the third component of the vector (z-dimension).
   * @param z The value to set.
   */
  void setZ(T z) {
    this->d[2] = z;
  }

  double distanceTo(const Vector3& target) const {
    double x = this->d[0] - target[0];
    double y = this->d[1] - target[1];
    double z = this->d[2] - target[2];
    return sqrt(x * x + y * y + z * z);
  }

  /**
   * Tests if this vector represents a point in space at the left of another (this.x <= other.x).
   * @param other The point to compare with.
   * @return True, if this vector is at the left. False, Otherwise.
   */
  [[nodiscard]] bool atLeft(const Vector3& other) const {
    return this->x() <= other.x();
  }

  /**
   * Tests if this vector represents a point in space at the right of another (this.x > other.x).
   * @param other The point to compare with.
   * @return True, if this vector is at the right. False, Otherwise.
   */
  [[nodiscard]] bool atRight(const Vector3& other) const {
    return !this->atLeft(other);
  }

  /**
   * Tests if this vector represents a point in space at the back of another (this.z <= other.z).
   * @param other The point to compare with.
   * @return True, if this vector is at the back. False, Otherwise.
   */
  [[nodiscard]] bool atBack(const Vector3& other) const {
    return this->z() <= other.z();
  }

  /**
   * Tests if this vector represents a point in space in front of another (this.z > other.z).
   * @param other The point to compare with.
   * @return True, if this vector is in front of. False, Otherwise.
   */
  [[nodiscard]] bool atFront(const Vector3& other) const {
    return !this->atBack(other);
  }

  /**
   * Tests if this vector represents a point in space at the down of another (this.y <= other.y).
   * @param other The point to compare with.
   * @return True, if this vector is at the down. False, Otherwise.
   */
  [[nodiscard]] bool atDown(const Vector3& other) const {
    return this->y() <= other.y();
  }

  /**
   * Tests if this vector represents a point in space above of another (this.y > other.y).
   * @param other The point to compare with.
   * @return True, if this vector is above. False, Otherwise.
   */
  [[nodiscard]] bool atUp(const Vector3& other) const {
    return !this->atDown(other);
  }

  /**
   * Calculates the length (norm) of the vector.
   * @return The length (norm) of the vector.
   */
  [[nodiscard]] double norm() const {
    return sqrt(
        pow(this->x(), 2) +
        pow(this->y(), 2) +
        pow(this->z(), 2)
    );
  }

  /**
   * Normalizes the vector: scales its components so its length becomes 1.
   */
  void normalize() {
    T norm = this->norm();
    this->d[0] /= norm;
    this->d[1] /= norm;
    this->d[2] /= norm;
  }

  /**
   * Calculates a hash value from the components of this vector.
   * @return The hash value.
   */
  [[nodiscard]] unsigned long hash() const {
    return phmap::HashState::combine(0, this->x(), this->y(), this->z());
  }

  /**
   * Adds the components of 2 vectors.
   * @param rhs The vector to add,
   * @return A new vector resulting from the sum of the components.
   */
  Vector3 operator+(const Vector3& rhs) const {
    auto ret = Vector3(*this);
    ret[0] += rhs[0];
    ret[1] += rhs[1];
    ret[2] += rhs[2];
    return ret;
  }

  /**
   * Subtracts the components of 2 vectors.
   * @param rhs The vector to subtract,
   * @return A new vector resulting from the difference of the components.
   */
  Vector3 operator-(const Vector3& rhs) const {
    auto ret = Vector3(*this);
    ret[0] -= rhs[0];
    ret[1] -= rhs[1];
    ret[2] -= rhs[2];
    return ret;
  }

  /**
   * Multiplies the components of 2 vectors.
   * @param rhs The vector to multiply,
   * @return A new vector resulting from the product of the components.
   */
  Vector3 operator*(const Vector3& rhs) const {
    auto ret = Vector3(*this);
    ret[0] *= rhs[0];
    ret[1] *= rhs[1];
    ret[2] *= rhs[2];
    return ret;
  }

  /**
   * Multiplies the components of 2 vectors.
   * @param rhs The vector to multiply,
   * @return A new vector resulting from the multiplication of the components.
   */
  Vector3 operator*(const T factor) const {
    auto ret = Vector3(*this);
    ret[0] *= factor;
    ret[1] *= factor;
    ret[2] *= factor;
    return ret;
  }

  /**
   * Divides the components of 2 vectors.
   * @param rhs The vector to divide,
   * @return A new vector resulting from the division of the components.
   */
  Vector3 operator/(const T factor) const {
    return *this * (1.0 / factor);
  }

  /**
   * Compares 2 vectors.
   * @param rhs The vector to compare against.
   * @return True if all components of both vectors are strictly equal, pairwise. False, otherwise.
   */
  bool operator==(const Vector3& rhs) const {
    for (int i = 0; i < 3; ++i) {
      if ((*this)[i] != rhs[i])
        return false;
    }
    return true;
  }

  /**
   * Compares 2 vectors.
   * @param rhs The vector to compare against.
   * @return False if all components of both vectors are strictly equal, pairwise. False, otherwise.
   */
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
    bool operator()(const Vector3<T>& a, const Vector3<T>& b) const {
      return a == b;
    }
  };

  struct Hash {
    unsigned long operator()(const Vector3<T>& v) const {
      return v.hash();
    }
  };
};

using Vector3d = Vector3<double>;
using Vector3f = Vector3<float>;
using Vector3i = Vector3<int>;
}

#endif //SLAM_VECTOR3_H
