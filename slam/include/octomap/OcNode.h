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
  template<class T>
  class OcNode {
  public:
    // Can return +/-infinite
    constexpr static double prob2logodds(double prob) {
      return log(prob / (1 - prob));
    }

    constexpr static double logodds2prob(double logodds) {
      return 1.0 - (1.0 / (1.0 + exp(logodds)));
    }

  private:
    using Key = OcNodeKey<T>;

    inline static double occThreshold = prob2logodds(0.5);
    inline static double minThreshold = prob2logodds(0.1);
    inline static double maxThreshold = prob2logodds(0.9);

    OcNode** children = nullptr;
    float logOdds;

    void allocChildren() {
      this->children = new OcNode* [8]{nullptr};
    }

    void expandNode() {
      assert(!this->hasChildren());
      if (this->children == nullptr) {
        this->allocChildren();
      }

      for (int i = 0; i < 8; ++i) {
        this->children[i] = new OcNode(this->getLogOdds());
      }
    }

    // Node is *prunable* is all children exist and have the same occupancy
    [[nodiscard]] bool isPrunable() const {
      OcNode* firstChild = this->getChild(0);
      if (firstChild == nullptr || firstChild->hasChildren()) return false;

      for (int i = 1; i < 8; ++i) {
        OcNode* child = this->getChild(i);
        if (child == nullptr ||             // all children exist
            child->hasChildren() ||         // they don't have children of their own
            (*firstChild) != (*child)) {    // and have the same occupancy
          return false;
        }
      }

      return true;
    }

    // Returns the max occupancy log-odds from the children.
    // For internal use: presence of all children should be checked first.
    [[nodiscard]] float getMaxChildrenLogOdds() const {
      float max = std::numeric_limits<float>::min();
      for (int i = 0; i < 8; ++i) {
        OcNode* child = this->children[i];
        if (child == nullptr) continue;
        if (child->getLogOdds() > max) max = child->getLogOdds();
      }
      return max;
    }

    // Returns the mean occupancy log-odds from the children.
    // For internal use: presence of all children should be checked first.
    [[nodiscard]] float getMeanChildrenLogOdds() const {
      unsigned int cnt = 0;
      float ret = 0;
      for (int i = 0; i < 8; ++i) {
        OcNode* child = this->children[i];
        if (child == nullptr) continue;
        ret += child->getLogOdds();
        ++cnt;
      }
      return ret / (float) cnt;
    }

    // Use the max of the children's occupancy (conservative approach).
    void updateBasedOnChildren() {
      if (this->children == nullptr) return;
      this->setLogOdds(this->getMaxChildrenLogOdds());
    }

    OcNode*
    setOrUpdateLogOdds(const Key& key, unsigned int depth, float lo, bool isUpdate, bool lazy = false,
                       bool justCreated = false) {
      bool createdChild = false;
      OcNode* child;

      // follow down to last level
      if (depth > 0) {
        unsigned int d = depth - 1;
        unsigned int pos = key.getStep(d);
        if (!this->childExists(pos)) {
          // child does not exist, but maybe it's a pruned node?
          if (!this->hasChildren() && !justCreated) {
            // current node does not have children AND it is not a new node
            // -> expand pruned node
            this->expandNode();
          } else {
            // not a pruned node, create requested child
            this->createChild(pos);
            createdChild = true;
          }
        }

        child = this->getChild(pos);
        child->setOrUpdateLogOdds(key, d, lo, isUpdate, lazy, createdChild);

        if (!lazy) {
          // prune if possible (return self if pruned)
          if (this->prune()) return this;
          // updated occupancy if not pruned (still has children)
          this->updateBasedOnChildren();
        }

        return child;
      } else { // at last level, update node, end of recursion
        if (isUpdate) this->updateLogOdds(lo);
        else this->setLogOdds(lo);
        return this;
      }
    }

    void writeBinaryInner(std::ostream& os, int baseI, std::bitset<8>& childBitset) const {
      // 00 : child is unknown node
      // 01 : child is occupied node
      // 10 : child is free node
      // 11 : child has children

      for (unsigned int i = 0; i < 4; ++i) {
        if (this->childExists(baseI + i)) {
          const OcNode* child = this->getChild(baseI + i);
          if (child->hasChildren()) {
            // 11 : child has children
            childBitset[i * 2] = 1;
            childBitset[i * 2 + 1] = 1;
          } else if (child->isOccupied()) {
            // 01 : child is occupied node
            childBitset[i * 2] = 0;
            childBitset[i * 2 + 1] = 1;
          } else {
            // 10 : child is free node
            childBitset[i * 2] = 1;
            childBitset[i * 2 + 1] = 0;
          }
        } else {
          // 00 : child is unknown node
          childBitset[i * 2] = 0;
          childBitset[i * 2 + 1] = 0;
        }
      }
    }

  public:
    explicit OcNode(float logOdds) : logOdds(logOdds) {}

    OcNode() : OcNode(OcNode::occThreshold) {}

    ~OcNode() {
      if (this->children != nullptr) {
        for (int i = 0; i < 8; ++i) {
          delete this->children[i];
        }
      }
      delete[] this->children;
    }

    [[nodiscard]] unsigned int getChildCount() const {
      if (this->children == nullptr) return 0;
      unsigned int ret = 0;
      for (int i = 0; i < 8; ++i) {
        OcNode* child = this->children[i];
        if (child != nullptr) ret += child->getChildCount() + 1;
      }
      return ret;
    }

    [[nodiscard]] OcNode* getChild(unsigned int pos) const {
      assert(pos < 8);
      if (this->children == nullptr) return nullptr;
      return this->children[pos];
    }

    OcNode* createChild(unsigned int pos) {
      assert(pos < 8);
      if (this->children == nullptr) {
        this->allocChildren();
      }

      if (!this->childExists(pos)) this->children[pos] = new OcNode();
      return this->children[pos];
    }

    [[nodiscard]] bool hasChildren() const {
      if (this->children == nullptr) return false;
      for (int i = 0; i < 8; ++i) {
        if (this->children[i] != nullptr) return true;
      }
      return false;
    }

    [[nodiscard]] bool childExists(unsigned int i) const {
      if (this->children == nullptr) return false;
      return this->children[i] != nullptr;
    }

    // Prunes the node if it fulfills the isPrunable method
    bool prune() {
      if (!this->isPrunable()) return false;

      // all children are equal so we take their value
      this->setLogOdds(this->getChild(0)->getLogOdds());
      // delete children
      for (int i = 0; i < 8; ++i) {
        delete this->children[i];
        this->children[i] = nullptr;
      }

      delete[] this->children;
      this->children = nullptr;

      return true;
    }

    [[nodiscard]] float getLogOdds() const {
      return this->logOdds;
    }

    [[nodiscard]] float getOccupancy() const {
      return (float) logodds2prob(this->logOdds);
    }

    // Sets the node occupancy log-odds. Performs min/max clamping.
    void setLogOdds(float lo) {
      this->logOdds = std::clamp(lo, (float) OcNode::minThreshold, (float) OcNode::maxThreshold);
    }

    void setOccupancy(float occ) {
      this->setLogOdds((float) prob2logodds(occ));
    }

    void updateLogOdds(float newLogOdds) {
      this->setLogOdds(this->logOdds + newLogOdds);
    }

    [[nodiscard]] bool isOccupied() const {
      return this->logOdds >= (float) OcNode::occThreshold;
    }

    [[nodiscard]] bool isOccupiedStable() const {
      return this->logOdds == (float) OcNode::maxThreshold;
    }

    [[nodiscard]] bool isFree() const {
      return !this->isOccupied();
    }

    [[nodiscard]] bool isFreeStable() const {
      return this->logOdds == (float) OcNode::minThreshold;
    }

    [[nodiscard]] bool wouldChange(float lo) const {
      if (lo == (float) OcNode::occThreshold) return false;
      if (lo < 0) {
        if (this->isFreeStable()) return false;
      } else if (lo > 0) {
        if (this->isOccupiedStable()) return false;
      }
      return true;
    }

    OcNode* setLogOdds(const Key& key, unsigned int depth, float lo, bool lazy = false, bool justCreated = false) {
      return this->setOrUpdateLogOdds(key, depth, lo, false, lazy, justCreated);
    }

    OcNode* updateLogOdds(const Key& key, unsigned int depth, float lo, bool lazy = false, bool justCreated = false) {
      return this->setOrUpdateLogOdds(key, depth, lo, true, lazy, justCreated);
    }

    OcNode* setOccupancy(const Key& key, unsigned int depth, float occ, bool lazy = false, bool justCreated = false) {
      return this->setLogOdds(key, depth, (float) prob2logodds(occ), lazy, justCreated);
    }

    OcNode*
    updateOccupancy(const Key& key, unsigned int depth, float occ, bool lazy = false, bool justCreated = false) {
      return this->updateLogOdds(key, depth, (float) prob2logodds(occ), lazy, justCreated);
    }

    // Prune children (if possible) and update the log-odds (if inner node).
    // Fixes the tree recursively, and should be called after a batch of lazy sets/updates.
    void fix() {
      for (int i = 0; i < 8; ++i) {
        auto child = this->getChild(i);
        if (child) child->fix();
      }

      // prune if possible
      if (this->prune()) return;
      // updated occupancy if not pruned
      this->updateBasedOnChildren();
    }

    OcNode* search(const Key& key, unsigned int depth) {
      if (depth > 0) {
        unsigned int d = depth - 1;
        unsigned int pos = key.getStep(d);
        OcNode* child = this->getChild(pos);
        if (child != nullptr) // child exists
          return child->search(key, d);
        else if (!this->hasChildren()) // we're a leaf (children pruned)
          return this;
        else // search failed
          return nullptr;
      } else {
        return this;
      }
    }

    bool operator==(const OcNode& rhs) const {
      return this->logOdds == rhs.logOdds;
    }

    bool operator!=(const OcNode& rhs) const {
      return !(rhs == *this);
    }

    void writeBinary(std::ostream& os) const {
      std::bitset<8> child1to4;
      std::bitset<8> child5to8;

      this->writeBinaryInner(os, 0, child1to4);
      this->writeBinaryInner(os, 4, child5to8);

      char child1to4_char = (char) child1to4.to_ulong();
      char child5to8_char = (char) child5to8.to_ulong();

      os.write((char*) &child1to4_char, sizeof(char));
      os.write((char*) &child5to8_char, sizeof(char));

      // write children
      for (unsigned int i = 0; i < 8; i++) {
        if (this->childExists(i)) {
          const OcNode* child = this->getChild(i);
          if (child->hasChildren()) child->writeBinary(os);
        }
      }
    }
  };
}

#endif //SLAM_OCNODE_H
