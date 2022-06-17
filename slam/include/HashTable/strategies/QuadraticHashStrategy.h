#ifndef SLAM_QUADRATICHASHSTRATEGY_H
#define SLAM_QUADRATICHASHSTRATEGY_H

#include "HashStrategy.h"

namespace HashTable {
  template<typename T>
  class QuadraticHashStrategy : public HashStrategy<T> {
  public:
    [[nodiscard]] size_t offset(size_t hash, int nIters) const override {
      return (nIters * nIters + nIters) / 2;
    }
  };
}

#endif //SLAM_QUADRATICHASHSTRATEGY_H
