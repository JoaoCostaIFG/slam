//
// Created by joao on 3/22/22.
//

#ifndef SLAM_VECTOR3_H
#define SLAM_VECTOR3_H

#include <ostream>

namespace octomap {
    class Vector3 {
    private:
        float d[3];
    public:
        Vector3(float x, float y, float z) {
            d[0] = x;
            d[1] = y;
            d[2] = z;
        }

        const float &operator[](unsigned int i) const {
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
