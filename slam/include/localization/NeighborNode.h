#ifndef SLAM_NEIGHBORNODE_H
#define SLAM_NEIGHBORNODE_H

#include "../octomap/Vector3.h"
#include "../octomap/OcNode.h"

namespace localization {

  template<typename T>
  class NeighborNode {
  public:
    const octomap::Vector3<> position;
    const octomap::OcNode<T>* node;

    NeighborNode(const octomap::Vector3<>& pos, const octomap::OcNode<T>* n) :
        position(pos),
        node(n) {
    }

    /**
     * Calculates a hash value.
     * @return The hash value.
     */
    [[nodiscard]] unsigned long hash() const {
      return position.hash();
    }

    struct Cmp {
      bool operator()(const NeighborNode<T>& a, const NeighborNode<T>& b) const {
        return a.position == b.position;
      }
    };

    struct Hash {
      unsigned long operator()(const NeighborNode<T>& n) const {
        return n.hash();
      }
    };
  };

}

#endif //SLAM_NEIGHBORNODE_H
