//
// Created by joao on 3/22/22.
//

#ifndef SLAM_VECTOR3_H
#define SLAM_VECTOR3_H

#include <ostream>
#include <cassert>

namespace octomap {
    class Vector3 {
    private:
        float d[3];
    public:
        Vector3(float x, float y, float z) : d{x, y, z} {}

        Vector3() : d{0, 0, 0} {}

        explicit Vector3(float n) : d{n, n, n} {}

        // copy constructor
        Vector3(Vector3 &o) : Vector3(o.x(), o.y(), o.z()) {}

        const float &operator[](unsigned int i) const {
            assert(i < 3);
            return d[i];
        }

        [[nodiscard]] float x() const {
            return this->operator[](0);
        }

        [[nodiscard]] float y() const {
            return this->operator[](1);
        }

        [[nodiscard]] float z() const {
            return this->operator[](2);
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

        bool operator==(const Vector3 &rhs) const {
            for (int i = 0; i < 3; ++i) {
                if (this->operator[](i) != rhs[i])
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
