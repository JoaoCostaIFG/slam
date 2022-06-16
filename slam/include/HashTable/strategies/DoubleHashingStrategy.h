//
// Created by PC on 08/06/2022.
//

#ifndef SLAM_DOUBLEHASHINGSTRATEGY_H
#define SLAM_DOUBLEHASHINGSTRATEGY_H

#include <stdint.h>
#include "HashStrategy.h"

template<typename T>
class DoubleHashingStrategy : public HashStrategy<T>{
public:
  [[nodiscard]] size_t nextHash(int index, size_t hash, int nIters)  const override{
    return hash * (nIters + 1);
  }
};


#endif //SLAM_DOUBLEHASHINGSTRATEGY_H
