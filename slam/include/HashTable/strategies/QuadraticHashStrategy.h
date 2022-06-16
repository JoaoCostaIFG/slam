#ifndef SLAM_QUADRATICHASHSTRATEGY_H
#define SLAM_QUADRATICHASHSTRATEGY_H

#include <stdint.h>
#include "HashStrategy.h"

namespace HashTable {
  template<typename T>
  class QuadraticHashStrategy : public HashStrategy<T> {
  public:
    [[nodiscard]] size_t offset(size_t hash, int nIters) const override {
      return nIters * nIters;
    }
  };
}

#endif //SLAM_QUADRATICHASHSTRATEGY_H
