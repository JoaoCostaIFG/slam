//
// Created by joao on 3/30/22.
//

#ifndef SLAM_OCTOMAPITERATOR_H
#define SLAM_OCTOMAPITERATOR_H

#include <stack>

#include "OcNode.h"

namespace octomap {
  template<class NODE>
  class OctomapIterator {
  private:
    std::stack<NODE*> nodeStack;

    // Return false if there is no next node
    bool nextNode() {
      if (this->nodeStack.empty()) return false;

      NODE* currentNode = this->nodeStack.top();
      this->nodeStack.pop();
      if (currentNode->hasChildren()) {
        for (int i = 7; i >= 0; --i) {
          NODE* childNode = currentNode->getChild(i);
          if (childNode) this->nodeStack.push(childNode);
        }
      }

      return !this->nodeStack.empty();
    }

  public:
    explicit OctomapIterator(NODE* node) {
      if (node != nullptr)
        this->nodeStack.push(node);
    }

    OctomapIterator operator++(int) {
      OctomapIterator result = *this;
      ++(*this);
      return result;
    }

    OctomapIterator operator++() {
      if (this->nextNode())
        return *this;
      return OctomapIterator(nullptr);
    }

    NODE* operator*() const {
      if (this->nodeStack.empty()) return nullptr;
      return this->nodeStack.top();
    }

    NODE* operator->() const {
      if (this->nodeStack.empty()) return nullptr;
      return this->nodeStack.top();
    }

    bool operator==(const OctomapIterator& rhs) const {
      return **this == *rhs;
    }

    bool operator!=(const OctomapIterator& rhs) const {
      return !(rhs == *this);
    }
  };
}

#endif //SLAM_OCTOMAPITERATOR_H
