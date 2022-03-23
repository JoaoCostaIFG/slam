//
// Created by joao on 3/22/22.
//

#ifndef SLAM_OCNODE_H
#define SLAM_OCNODE_H

#include <bitset>
#include <cassert>
#include <cmath>
#include <ostream>

#include "OcNodeKey.h"

#define OCCUP_UNKOWN 0.5f

namespace octomap {
    class OcNode {
    private:
        OcNode **children = nullptr;
        // TODO store log-odds instead of probability (see funcs bellow)
        float occupancy;

        void allocChildren();

        void expandNode();

        // uses max occupancy from children
        void updateOccBasedOnChildren();

        void writeBinaryInner(std::ostream &os, int baseI, std::bitset<8> &childBitset) const;

    public:
        explicit OcNode(float occ);

        OcNode();

        ~OcNode();

        [[nodiscard]] unsigned int getChildCount() const;

        [[nodiscard]] OcNode *getChild(unsigned int pos) const;

        OcNode *createChild(unsigned int pos);

        [[nodiscard]] bool hasChildren() const;

        [[nodiscard]] bool childExists(unsigned int i) const;

        [[nodiscard]] float getOccupancy() const {
            return this->occupancy;
        }

        [[nodiscard]] bool isOccupied() const {
            return this->occupancy > OCCUP_UNKOWN;
        }

        [[nodiscard]] bool isFree() const {
            return !this->isOccupied();
        }

        void setOccupancy(float occ) {
            this->occupancy = occ;
        }

        OcNode* setOccupancy(const OcNodeKey &key, unsigned int depth, float occ, bool justCreated = false);

        static double prob2logodds(double prob) {
            return log(prob / (1 - prob));
        }

        static double logodds2prob2(double logodds) {
            return 1.0 - (1.0 / (1.0 + exp(logodds)));
        }

        void writeBinary(std::ostream &os) const;
    };
}

#endif //SLAM_OCNODE_H
