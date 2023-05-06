#ifndef SLAM_LINEARHASHSTRATEGY_H
#define SLAM_LINEARHASHSTRATEGY_H

#include <stdint.h>
#include "HashStrategy.h"

namespace HashTable {
  template<typename T>
  class LinearHashStrategy : public HashStrategy<T> {
  public:
    [[nodiscard]] size_t offset(size_t hash, int nIters) const override {
      return nIters;
    }
  };
}

#endif //SLAM_LINEARHASHSTRATEGY_H
