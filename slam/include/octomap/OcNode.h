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

namespace octomap {
    class OcNode {
    public:
        // Can return +/-infinite
        static double prob2logodds(double prob) {
            return log(prob / (1 - prob));
        }

        static double logodds2prob(double logodds) {
            return 1.0 - (1.0 / (1.0 + exp(logodds)));
        }

    private:
        inline static double occThreshold = prob2logodds(0.5);
        inline static double minThreshold = prob2logodds(0.1);
        inline static double maxThreshold = prob2logodds(0.9);

        OcNode **children = nullptr;
        float logOdds;

        void allocChildren();

        void expandNode();

        // Node is prunable is all children exist and have the same occupancy
        [[nodiscard]] bool isPrunable() const;

        // Returns the max occupancy log-odds from the children.
        // For internal use: presence of all children should be checked first.
        [[nodiscard]] float getMaxChildrenLogOdds() const;

        // Returns the mean occupancy log-odds from the children.
        // For internal use: presence of all children should be checked first.
        [[nodiscard]] float getMeanChildrenLogOdds() const;

        // Use the max of the children's occupancy (conservative approach).
        void updateBasedOnChildren();

        OcNode *setOrUpdateLogOdds(const OcNodeKey &key, unsigned int depth, float lo, bool isUpdate, bool justCreated = false);

        void writeBinaryInner(std::ostream &os, int baseI, std::bitset<8> &childBitset) const;

    public:
        explicit OcNode(float logOdds);

        OcNode();

        ~OcNode();

        [[nodiscard]] unsigned int getChildCount() const;

        [[nodiscard]] OcNode *getChild(unsigned int pos) const;

        OcNode *createChild(unsigned int pos);

        [[nodiscard]] bool hasChildren() const;

        [[nodiscard]] bool childExists(unsigned int i) const;

        bool prune();

        [[nodiscard]] float getLogOdds() const {
            return this->logOdds;
        }

        [[nodiscard]] float getOccupancy() const {
            return (float) logodds2prob(this->logOdds);
        }

        // Sets the node occupancy log-odds. Performs min/max clamping.
        void setLogOdds(float lo) {
            if (lo >= OcNode::maxThreshold)
                this->logOdds = (float) OcNode::maxThreshold;
            else if (lo <= OcNode::minThreshold)
                this->logOdds = (float) OcNode::minThreshold;
            else
                this->logOdds = lo;
        }

        void setOccupancy(float occ) {
            this->setLogOdds((float) prob2logodds(occ));
        }

        void updateLogOdds(float newLogOdds) {
            this->setLogOdds(this->logOdds + newLogOdds);
        }

        [[nodiscard]] bool isOccupied() const {
            return this->logOdds >= OcNode::occThreshold;
        }

        [[nodiscard]] bool isFree() const {
            return !this->isOccupied();
        }

        OcNode *setLogOdds(const OcNodeKey &key, unsigned int depth, float lo, bool justCreated = false);

        OcNode *updateLogOdds(const OcNodeKey &key, unsigned int depth, float lo, bool justCreated = false);

        OcNode *setOccupancy(const OcNodeKey &key, unsigned int depth, float occ, bool justCreated = false);

        OcNode *updateOccupancy(const OcNodeKey &key, unsigned int depth, float occ, bool justCreated = false);

        bool operator==(const OcNode &rhs) const {
            return this->logOdds == rhs.logOdds;
        }

        bool operator!=(const OcNode &rhs) const {
            return !(rhs == *this);
        }

        void writeBinary(std::ostream &os) const;
    };
}

#endif //SLAM_OCNODE_H
