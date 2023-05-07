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
};

}

#endif //SLAM_NEIGHBORNODE_H
