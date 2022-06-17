#ifndef SLAM_HASHSTRATEGY_H
#define SLAM_HASHSTRATEGY_H

#include <stdint.h>

namespace HashTable {
  template<typename T>
  class HashStrategy {
  public:
    virtual ~HashStrategy() {}

    unsigned long hash(const T& elem) const {
      return elem.hash();
    }

    virtual size_t offset(size_t hash, int nIters) const = 0;
  };
}

#endif //SLAM_HASHSTRATEGY_H
