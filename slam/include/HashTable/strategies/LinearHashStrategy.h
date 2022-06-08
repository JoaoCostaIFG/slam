//
// Created by PC on 08/06/2022.
//

#ifndef SLAM_LINEARHASHSTRATEGY_H
#define SLAM_LINEARHASHSTRATEGY_H

#include <stdint.h>
#include "HashStrategy.h"

template<typename T>
class LinearHashStrategy : public HashStrategy<T>{
public:
  [[nodiscard]] size_t nextHash(int index, size_t hash, int nIters) const override{
    return index + 1;
  }
};


#endif //SLAM_LINEARHASHSTRATEGY_H
