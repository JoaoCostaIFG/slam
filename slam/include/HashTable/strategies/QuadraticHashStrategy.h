//
// Created by PC on 08/06/2022.
//

#ifndef SLAM_QUADRATICHASHSTRATEGY_H
#define SLAM_QUADRATICHASHSTRATEGY_H

#include <stdint.h>
#include "HashStrategy.h"

template<typename T>
class QuadraticHashStrategy : public HashStrategy<T> {
public:
  [[nodiscard]] size_t nextHash(int index, size_t hash, int nIters)  const override{
    return index + nIters*nIters;
  }
};


#endif //SLAM_QUADRATICHASHSTRATEGY_H
