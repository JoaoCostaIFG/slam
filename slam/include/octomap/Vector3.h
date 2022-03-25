//
// Created by joao on 3/22/22.
//

#ifndef SLAM_VECTOR3_H
#define SLAM_VECTOR3_H

#include <ostream>
#include <cassert>
#include <cmath>

namespace octomap {
    class Vector3 {
    private:
        float d[3];
    public:
        Vector3(float x, float y, float z) : d{x, y, z} {}

        Vector3() : d{0, 0, 0} {}

        explicit Vector3(float n) : d{n, n, n} {}

        // copy constructor
        Vector3(const Vector3 &o) : Vector3(o.x(), o.y(), o.z()) {}

        const float &operator[](unsigned int i) const {
            assert(i < 3);
            return this->d[i];
        }

        float &operator[](unsigned int i) {
            assert(i < 3);
            return this->d[i];
        }


        [[nodiscard]] float x() const {
            return (*this)[0];
        }

        [[nodiscard]] float y() const {
            return (*this)[1];
        }

        [[nodiscard]] float z() const {
            return (*this)[2];
        }

        void setX(float x) {
            this->d[0] = x;
        }

        void setY(float y) {
            this->d[1] = y;
        }

        void setZ(float z) {
            this->d[2] = z;
        }

        [[nodiscard]] bool atLeft(const Vector3 &other) const {
            return this->x() <= other.x();
        }

        [[nodiscard]] bool atRight(const Vector3 &other) const {
            return !this->atLeft(other);
        }

        [[nodiscard]] bool atBack(const Vector3 &other) const {
            return this->z() <= other.z();
        }

        [[nodiscard]] bool atFront(const Vector3 &other) const {
            return !this->atBack(other);
        }

        [[nodiscard]] bool atDown(const Vector3 &other) const {
            return this->y() <= other.y();
        }

        [[nodiscard]] bool atUp(const Vector3 &other) const {
            return !this->atDown(other);
        }

        [[nodiscard]] float norm() const {
            return (float) sqrt(pow(this->x(), 2) + pow(this->y(), 2) + pow(this->z(), 2))
        }

        void normalize() {
            float norm = this->norm();
            this->d[0] /= norm;
            this->d[1] /= norm;
            this->d[2] /= norm;
        }

        Vector3 operator+(const Vector3 &rhs) const {
            auto ret = Vector3(*this);
            ret[0] += rhs[0];
            ret[1] += rhs[1];
            ret[2] += rhs[2];
            return ret;
        }

        Vector3 operator-(const Vector3 &rhs) const {
            auto ret = Vector3(*this);
            ret[0] -= rhs[0];
            ret[1] -= rhs[1];
            ret[2] -= rhs[2];
            return ret;
        }

        Vector3 operator*(const float factor) const {
            auto ret = Vector3(*this);
            ret[0] *= factor;
            ret[1] *= factor;
            ret[2] *= factor;
            return ret;
        }

        Vector3 operator/(const float factor) const {
            return *this * float(1.0 / factor);
        }

        bool operator==(const Vector3 &rhs) const {
            for (int i = 0; i < 3; ++i) {
                if ((*this)[i] != rhs[i])
                    return false;
            }
            return true;
        }

        bool operator!=(const Vector3 &rhs) const {
            return !(rhs == *this);
        }

        friend std::ostream &operator<<(std::ostream &out, Vector3 const &v) {
            return out <<
                       "(" << v.x() << " " << v.y() << " " << v.z() << ")";
        }
    };
}

#endif //SLAM_VECTOR3_H
