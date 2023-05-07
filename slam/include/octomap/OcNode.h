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
    /**
     * Converts the given probability (occupancy) to log-odds.
     * @warning Can return +/-infinite
     * @param prob The probability to convert.
     * @return The converted log-odds value.
     */
    constexpr static double prob2logodds(double prob) {
      return log(prob / (1 - prob));
    }

    /**
     * Converts the given log-odds to the corresponding probability (occupancy).
     * @param logodds The log-odds to convert.
     * @return The converted probabilities value.
     */
    constexpr static double logodds2prob(double logodds) {
      return 1.0 - (1.0 / (1.0 + exp(logodds)));
    }

  protected:
    OcNode** children = nullptr;

  private:
    using Key = OcNodeKey<T>;

    inline static double occThreshold = prob2logodds(0.85);
    inline static double minThreshold = prob2logodds(0.1);
    inline static double maxThreshold = prob2logodds(0.9);

    double logOdds;

    /**
     * Helper method that allocates the container for the 8 children of this node.
     * @warning This doesn't check if the container is already instantiated.
     */
    void allocChildren() {
      this->children = new OcNode* [8]{nullptr};
    }

    /**
     * Helper method that allocates 8 children for the current node (and their container if needed),
     * and initializes them with the log-odds value of the current node (parent).
     */
    void expandNode() {
      assert(!this->hasChildren());
      if (this->children == nullptr) {
        this->allocChildren();
      }

      for (int i = 0; i < 8; ++i) {
        this->children[i] = new OcNode(this->getLogOdds());
      }
    }

    /**
     * Checks if the current node is "prunable".
     * A node is "prunable" is it has 8 children with strictly the same occupancy.
     * @return True if the node is prunable. False, otherwise.
     */
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

    /**
     * Helper method that calculates the maximum log-odds value of the node's children.
     * @warning The presence of all children should be checked first.
     * @return The maximum log-odds value of the node's children.
     */
    [[nodiscard]] double getMaxChildrenLogOdds() const {
      double max = std::numeric_limits<double>::min();
      for (int i = 0; i < 8; ++i) {
        OcNode* child = this->children[i];
        if (child == nullptr) continue;
        if (child->getLogOdds() > max) max = child->getLogOdds();
      }
      return max;
    }

    /**
     * Helper method that calculates the mean log-odds value of the node's children.
     * @warning The presence of all children should be checked first.
     * @return The mean log-odds value of the node's children.
     */
    [[nodiscard]] double getMeanChildrenLogOdds() const {
      unsigned int cnt = 0;
      double ret = 0;
      for (int i = 0; i < 8; ++i) {
        OcNode* child = this->children[i];
        if (child == nullptr) continue;
        ret += child->getLogOdds();
        ++cnt;
      }
      return ret / (double) cnt;
    }

    /**
     * Helper method that updates the node's log-odds using the maximum log-odds of its children.
     * The mean strategy could be used alternatively (max is the conservative approach).
     */
    void updateBasedOnChildren() {
      if (this->children == nullptr) return;
      this->setLogOdds(this->getMaxChildrenLogOdds());
    }

    /**
     * Sets/updates the log-odds value of the current node. If the current node isn't at the lowest level
     * (leaf node), the update function of a child is called (recursive).
     * @param key The key representing the target node.
     * @param depth The current depth on the tree (counts backwards => 0 is the lowest level).
     * @param lo The log-odds value to use.
     * @param isUpdate Whether this is an update (true) or a set (false).
     * @param lazy Whether to do a lazy update (default=false).
     * @param justCreated Whether this node was created because of this update (default=false).
     * @return A pointer to the (final) updated node.
     */
    OcNode*
    setOrUpdateLogOdds(const Key& key, unsigned int depth, double lo, bool isUpdate,
                       bool lazy = false,
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

    /**
     * Helper method to convert the current node to a binary format compatible with octoviz.
     * @param baseI What children to start with.
     * @param childBitset The output in binary.
     */
    void writeBinaryInner(int baseI, std::bitset<8>& childBitset) const {
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
    explicit OcNode(double logOdds) : logOdds(logOdds) {}

    OcNode() : OcNode(OcNode::occThreshold) {}

    ~OcNode() {
      if (this->children != nullptr) {
        for (int i = 0; i < 8; ++i) {
          delete this->children[i];
        }
      }
      delete[] this->children;
    }

    /**
     * Counts the number of children of this node (direct and indirect).
     * @return The number of successors of this node.
     */
    [[nodiscard]] unsigned int getChildCount() const {
      if (this->children == nullptr) return 0;
      unsigned int ret = 0;
      for (int i = 0; i < 8; ++i) {
        OcNode* child = this->children[i];
        if (child != nullptr) ret += child->getChildCount() + 1;
      }
      return ret;
    }

    /**
     * Gets a pointer to a given child (if it exists).
     * @param pos The index of the child in the children container (pos < 8).
     * @return A pointer to the wanted children. nullptr if it doesn't exist/isn't instantiated.
     */
    [[nodiscard]] OcNode* getChild(unsigned int pos) const {
      assert(pos < 8);
      if (this->children == nullptr) return nullptr;
      return this->children[pos];
    }

    /**
     * Creates a child with the given index @param pos.
     * @param pos The index of the child in the children container (pos < 8).
     * @return A pointer to the new child.
     */
    OcNode* createChild(unsigned int pos) {
      assert(pos < 8);
      if (this->children == nullptr) {
        this->allocChildren();
      }

      if (!this->childExists(pos)) this->children[pos] = new OcNode();
      return this->children[pos];
    }

    /**
     * Checks if the current node has any children (at least 1).
     * @return True, if the node has at least 1 child. False, otherwise.
     */
    [[nodiscard]] bool hasChildren() const {
      if (this->children == nullptr) return false;
      for (int i = 0; i < 8; ++i) {
        if (this->children[i] != nullptr) return true;
      }
      return false;
    }

    /**
     * Checks if a child with the given index, @param pos, exists.
     * @param pos The index of the child to check (pos < 8).
     * @return True, if the child exists. False, otherwise.
     */
    [[nodiscard]] bool childExists(unsigned int pos) const {
      if (this->children == nullptr) return false;
      return this->children[pos] != nullptr;
    }

    /**
     * Prunes the node (if possible). The isPrunable method is used to check if
     * the node is "prunable" beforehand.
     * @return True, if the node pruned children. False, otherwise.
     */
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

    [[nodiscard]] double getLogOdds() const {
      return this->logOdds;
    }

    [[nodiscard]] double getOccupancy() const {
      return logodds2prob(this->logOdds);
    }

    /**
     * Sets the log-odds value of the node. Performs min/max clamping.
     * @param lo The log-odds value to set.
     */
    void setLogOdds(double lo) {
      this->logOdds = std::clamp(lo, OcNode::minThreshold, OcNode::maxThreshold);
    }

    void setOccupancy(double occ) {
      this->setLogOdds(prob2logodds(occ));
    }

    void updateLogOdds(double newLogOdds) {
      this->setLogOdds(this->logOdds + newLogOdds);
    }

    [[nodiscard]] bool isOccupied() const {
      return this->logOdds >= OcNode::occThreshold;
    }

    [[nodiscard]] bool isOccupiedStable() const {
      return this->logOdds == OcNode::maxThreshold;
    }

    [[nodiscard]] bool isFree() const {
      return !this->isOccupied();
    }

    [[nodiscard]] bool isFreeStable() const {
      return this->logOdds == OcNode::minThreshold;
    }

    /**
     * Checks if the node would change if updated with the given log-odds value, @param lo.
     * A node won't change if @param lo is 0, or if the node is stable and the given value would
     * not affect this stability.
     * @param lo The log-odds value to test.
     * @return True, if the node would change. False, otherwise.
     */
    [[nodiscard]] bool wouldChange(double lo) const {
      if (lo == OcNode::occThreshold) return false;
      if (lo < 0) {
        if (this->isFreeStable()) return false;
      } else if (lo > 0) {
        if (this->isOccupiedStable()) return false;
      }
      return true;
    }

    OcNode* setLogOdds(const Key& key, unsigned int depth, double lo, bool lazy = false,
                       bool justCreated = false) {
      return this->setOrUpdateLogOdds(key, depth, lo, false, lazy, justCreated);
    }

    OcNode*
    updateLogOdds(const Key& key, unsigned int depth, double lo, bool lazy = false,
                  bool justCreated = false) {
      return this->setOrUpdateLogOdds(key, depth, lo, true, lazy, justCreated);
    }

    OcNode*
    setOccupancy(const Key& key, unsigned int depth, double occ, bool lazy = false,
                 bool justCreated = false) {
      return this->setLogOdds(key, depth, prob2logodds(occ), lazy, justCreated);
    }

    OcNode*
    updateOccupancy(const Key& key, unsigned int depth, double occ, bool lazy = false,
                    bool justCreated = false) {
      return this->updateLogOdds(key, depth, prob2logodds(occ), lazy,
                                 justCreated);
    }

    /**
     * Prune children (if possible) and update the log-odds (if inner node).
     * Fixes the tree recursively, and should be called after a batch of lazy sets/updates.
     */
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

    /**
     * Search for the node resented by the given key. Can cause recursive calls.
     * @param key The key representing the wanted node.
     * @param depth The current depth in the tree (counting backwards).
     * @return A pointer to the OcNode represented by the given key in the tree.
     */
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

    /**
     * Checks if 2 nodes are equal.
     * 2 OcNodes are equal if they have the same (strictly) log-odds.
     * @param rhs The node to compare against.
     * @return True, if the 2 OcNodes are equal. False, otherwise.
     */
    bool operator==(const OcNode& rhs) const {
      return this->logOdds == rhs.logOdds;
    }

    /**
     * Checks if 2 nodes are different.
     * 2 OcNodes are different if they have different (strictly) log-odds.
     * @param rhs The node to compare against.
     * @return False, if the 2 OcNodes are equal. True, otherwise.
     */
    bool operator!=(const OcNode& rhs) const {
      return !(rhs == *this);
    }

    /**
     * Writes the current node in binary format (compatible with octoviz) to the given output stream.
     * @note Writing the current node implies writing the children (recursive).
     * @param os The output stream to write to.
     */
    void writeBinary(std::ostream& os) const {
      std::bitset<8> child1to4;
      std::bitset<8> child5to8;

      this->writeBinaryInner(0, child1to4);
      this->writeBinaryInner(4, child5to8);

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
