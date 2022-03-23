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
    class OcNode {
    private:
        OcNode **children = nullptr;
        // TODO store log-odds instead of probability (see funcs bellow)
        float occupancy;

        void expandNode();

        void writeBinaryInner(std::ostream &os, int baseI, std::bitset<8> &childBitset) const;

    public:
        OcNode();

        ~OcNode();

        [[nodiscard]] OcNode *getChild(unsigned int pos) const;

        bool createChild(unsigned int pos);

        [[nodiscard]] bool hasChildren() const;

        [[nodiscard]] bool childExists(unsigned int i) const;

        [[nodiscard]] float getOccupancy() const {
            return this->occupancy;
        }

        void setOccupancy(float occ) {
            this->occupancy = occ;
        }

        [[nodiscard]] bool isOccupied() const {
            return this->occupancy > OCCUP_UNKOWN;
        }

        [[nodiscard]] bool isFree() const {
            return !this->isOccupied();
        }

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
