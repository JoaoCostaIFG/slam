//
// Created by PC on 08/06/2022.
//

#ifndef SLAM_HASHSTRATEGY_H
#define SLAM_HASHSTRATEGY_H

#include <stdint.h>

template<typename T>
class HashStrategy {
public:
  virtual ~HashStrategy() {}
  virtual size_t nextHash(int index, size_t hash, int nIters) const = 0;
};


#endif //SLAM_HASHSTRATEGY_H
