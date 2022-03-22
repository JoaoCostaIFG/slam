//
// Created by joao on 3/22/22.
//

#ifndef SLAM_OCNODE_H
#define SLAM_OCNODE_H

#include <bitset>
#include <cassert>
#include <cmath>
#include <ostream>

#define RIGHT 1
#define FRONT 2
#define UP    4

#define OCCUP_UNKOWN 0.5f

namespace octomap {
    class Ocnode {
    private:
        Ocnode *children = nullptr;
        // TODO store log-odds instead of probability (see funcs bellow)
        float occupancy;

        void writeBinaryInner(std::ostream &os, int baseI, std::bitset<8> &childBitset) const;

    public:
        Ocnode();

        ~Ocnode();

        Ocnode *getChild(unsigned int pos) const;

        bool hasChildren() const;

        bool childExists(unsigned int i) const;

        [[nodiscard]] float getOccupancy() const {
            return this->occupancy;
        }

        void setOccupancy(float occ) {
            this->occupancy = occ;
        }

        bool isOccupied() const {
            return this->occupancy > OCCUP_UNKOWN;
        }

        bool isFree() const {
            return !this->isOccupied();
        }

        void splitNode();

        static double prob2logodds(double prob) {
            return log(prob / (1 - prob));
        }

        static double logodds2prob2(double logodds) {
            return 1.0 - (1.0 / (1.0 + exp(logodds)));
        }

        void writeBinary(std::ostream &os)const;
    };
}

#endif //SLAM_OCNODE_H
