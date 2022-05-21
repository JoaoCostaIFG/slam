//
// Created by PC on 17/05/2022.
//

#ifndef SLAM_HASHTABLEITERATOR_H
#define SLAM_HASHTABLEITERATOR_H

#include "vector"

namespace HashTableIterator {
  template<typename TYPE>
  class HashTableIterator {
  private:
    std::vector<TYPE>* vec;
    size_t currInd;
  public:
    explicit HashTableIterator(std::vector<TYPE>* vec) {
      this->vec = vec;
      this->currInd = 0;
    }

    bool nextIndex() {
      while(currInd < this->vec->size()){
        currInd++;

        if(currInd >= this->vec->size()){
          return false;
        }

        if(vec->at(currInd) != nullptr) return true;
      }
      return false;
    }

    HashTableIterator operator++(int) {
      if(currInd >= this->vec->size()) return HashTableIterator(new std::vector<TYPE>());
      HashTableIterator result = *this;
      ++(*this);
      return result;
    }

    HashTableIterator operator++() {
      if (this->nextIndex())
        return *this;
      return HashTableIterator(new std::vector<TYPE>());
    }

    TYPE operator*() {
      if (done()) return this->vec->at(currInd-1);
      return this->vec->at(currInd);
    }

    TYPE operator->() {
      if (done()) return this->vec->at(currInd-1);
      return this->vec->at(currInd);
    }

    bool operator==(const HashTableIterator& rhs) const {
      return **this == *rhs;
    }

    bool operator!=(const HashTableIterator& rhs) const {
      return !(rhs == *this);
    }

    bool done(){
      return currInd >= this->vec->size();
    }

  };
}


#endif //SLAM_HASHTABLEITERATOR_H
