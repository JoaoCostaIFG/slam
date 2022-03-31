//
// Created by joao on 3/22/22.
//

#include <iostream>
#include "../include/octomap/OcNode.h"

using namespace octomap;


template<class KEY>
OcNode<KEY>::OcNode(float logOdds) : logOdds(logOdds) {}

template<class KEY>
OcNode<KEY>::OcNode() : OcNode<KEY>(OcNode::occThreshold) {}

template<class KEY>
OcNode<KEY>::~OcNode() {
  if (this->children != nullptr) {
    for (int i = 0; i < 8; ++i) {
      delete this->children[i];
    }
  }
  delete[] this->children;
}

template<class KEY>
unsigned int OcNode<KEY>::getChildCount() const {
  if (this->children == nullptr) return 0;
  unsigned int ret = 0;
  for (int i = 0; i < 8; ++i) {
    OcNode* child = this->children[i];
    if (child != nullptr) ret += child->getChildCount() + 1;
  }
  return ret;
}

template<class KEY>
OcNode<KEY>* OcNode<KEY>::getChild(unsigned int pos) const {
  assert(pos < 8);
  if (this->children == nullptr) return nullptr;
  return this->children[pos];
}

template<class KEY>
OcNode<KEY>* OcNode<KEY>::createChild(unsigned int pos) {
  assert(pos < 8);
  if (this->children == nullptr) {
    this->allocChildren();
  }

  if (!this->childExists(pos)) this->children[pos] = new OcNode();
  return this->children[pos];
}

template<class KEY>
void OcNode<KEY>::allocChildren() {
  this->children = new OcNode* [8]{nullptr};
}

template<class KEY>
void OcNode<KEY>::expandNode() {
  assert(!this->hasChildren());
  if (this->children == nullptr) {
    this->allocChildren();
  }

  for (int i = 0; i < 8; ++i) {
    this->children[i] = new OcNode(this->getLogOdds());
  }
}

template<class KEY>
bool OcNode<KEY>::hasChildren() const {
  if (this->children == nullptr) return false;
  for (int i = 0; i < 8; ++i) {
    if (this->children[i] != nullptr) return true;
  }
  return false;
}

template<class KEY>
bool OcNode<KEY>::childExists(unsigned int i) const {
  if (this->children == nullptr) return false;
  return this->children[i] != nullptr;
}

template<class KEY>
bool OcNode<KEY>::isPrunable() const {
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

template<class KEY>
bool OcNode<KEY>::prune() {
  if (!this->isPrunable()) return false;
  // all children are equal so we take their value
  this->setLogOdds(this->getChild(0)->getLogOdds());
  // delete children
  for (int i = 0; i < 8; ++i)
    delete this->children[i];
  delete[] this->children;
  this->children = nullptr;
  return true;
}

template<class KEY>
float OcNode<KEY>::getMaxChildrenLogOdds() const {
  float max = std::numeric_limits<float>::min();
  for (int i = 0; i < 8; ++i) {
    OcNode* child = this->children[i];
    if (child == nullptr) continue;
    if (child->getLogOdds() > max) max = child->getLogOdds();
  }
  return max;
}

template<class KEY>
float OcNode<KEY>::getMeanChildrenLogOdds() const {
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

template<class KEY>
void OcNode<KEY>::updateBasedOnChildren() {
  if (this->children == nullptr) return;
  this->setLogOdds(this->getMaxChildrenLogOdds());
}

template<class KEY>
OcNode<KEY>*
OcNode<KEY>::setOrUpdateLogOdds(const KEY& key, const unsigned int depth, const float lo,
                                const bool isUpdate, const bool justCreated) {
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
    child->setOrUpdateLogOdds(key, d, lo, isUpdate, createdChild);

    // prune if possible (return self is pruned)
    if (this->prune()) return this;
    // updated occupancy if not pruned
    this->updateBasedOnChildren();
    return child;
  } else { // at last level, update node, end of recursion
    if (isUpdate) this->updateLogOdds(lo);
    else this->setLogOdds(lo);
    return this;
  }
}

template<class KEY>
OcNode<KEY>* // suckless
OcNode<KEY>::setLogOdds(const KEY& key, const unsigned int depth, const float lo, const bool justCreated) {
  return this->setOrUpdateLogOdds(key, depth, lo, false, justCreated);
}

template<class KEY>
OcNode<KEY>* OcNode<KEY>::updateLogOdds(const KEY& key, unsigned int depth, float lo, bool justCreated) {
  return this->setOrUpdateLogOdds(key, depth, lo, true, justCreated);
}

template<class KEY>
OcNode<KEY>*
OcNode<KEY>::setOccupancy(const KEY& key, const unsigned int depth, const float occ, const bool justCreated) {
  return this->setLogOdds(key, depth, (float) prob2logodds(occ), justCreated);
}

template<class KEY>
OcNode<KEY>*
OcNode<KEY>::updateOccupancy(const KEY& key, const unsigned int depth, const float occ, const bool justCreated) {
  return this->updateLogOdds(key, depth, (float) prob2logodds(occ), justCreated);
}

template<class KEY>
OcNode<KEY>* OcNode<KEY>::search(const KEY& key, const unsigned int depth) {
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

template<class KEY>
void OcNode<KEY>::writeBinaryInner(std::ostream& os, int baseI, std::bitset<8>& childBitset) const {
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

template<class KEY>
void OcNode<KEY>::writeBinary(std::ostream& os) const {
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
