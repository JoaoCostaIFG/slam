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
    typename std::vector<TYPE>::iterator begin;
    typename std::vector<TYPE>::iterator end;
  public:
    explicit HashTableIterator(typename std::vector<TYPE>::iterator beg, typename std::vector<TYPE>::iterator en) {
      this->begin = beg;
      this->end = en;
    }

    bool nextIndex() {
      while(begin+1 != end){
        begin++;

        //this will most likely never happen, but just to be safe
        if(begin == end){
          return false;
        }
        if(*begin != nullptr) return true;
      }
      return false;
    }

    HashTableIterator operator++(int) {
      if(this->begin+1 == this->end) return HashTableIterator(this->end, this->end);
      HashTableIterator result = *this;
      ++(*this);
      return result;
    }

    HashTableIterator operator++() {
      if (this->nextIndex())
        return *this;
      std::cout << "Done" << std::endl;
      return HashTableIterator(this->end, this->end);
    }

    TYPE operator*() const {
      //if (this->begin == this->end) return nullptr; //TODO:isto causa problemas de comp, no entanto nunca acontece
      return *this->begin;
    }

    TYPE operator->() const {
      //if (this->begin == this->end) return nullptr; //TODO:isto causa problemas de comp, no entanto nunca acontece
      return *this->begin;
    }

    bool operator==(const HashTableIterator& rhs) const {
      return **this == *rhs;
    }

    bool operator!=(const HashTableIterator& rhs) const {
      return !(rhs == *this);
    }

    bool done(){
      return begin == end;
    }

  };
}


#endif //SLAM_HASHTABLEITERATOR_H
