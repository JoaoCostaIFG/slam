#ifndef SLAM_DOUBLEHASHINGSTRATEGY_H
#define SLAM_DOUBLEHASHINGSTRATEGY_H

#include <stdint.h>
#include "HashStrategy.h"

namespace HashTable {
  template<typename T>
  class DoubleHashingStrategy : public HashStrategy<T> {
  public:
    [[nodiscard]] size_t offset(size_t hash, int nIters) const override {
      return (hash | 1) * nIters;
    }
  };
}

#endif //SLAM_DOUBLEHASHINGSTRATEGY_H
