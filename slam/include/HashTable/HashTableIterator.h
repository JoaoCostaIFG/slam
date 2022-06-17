#ifndef SLAM_HASHTABLEITERATOR_H
#define SLAM_HASHTABLEITERATOR_H

#include "vector"

namespace HashTable {
  template<typename TYPE>
  class HashTableIterator {
  protected:
    bool isEnd;

  private:
    typename std::vector<TYPE>::const_iterator vecBegin;
    typename std::vector<TYPE>::const_iterator vecEnd;

    bool nextIndex() {
      if (vecBegin != vecEnd) {
        ++vecBegin;
        for (; vecBegin != vecEnd; ++vecBegin) {
          if (*vecBegin != nullptr && !(*vecBegin)->isDeleted())
            return true;
        }
      }

      this->isEnd = true;
      return false;
    }

  public:
    explicit HashTableIterator(const std::vector<TYPE>& vec, bool isEnd = false) : isEnd(isEnd) {
      this->vecBegin = vec.begin();
      this->vecEnd = vec.end();

      if (isEnd) {
        this->vecBegin = this->vecEnd;
      } else if (*(this->vecBegin) == nullptr || (*vecBegin)->isDeleted()) {
        this->nextIndex();
      }
    }

    HashTableIterator operator++() {
      if (!this->nextIndex())
        this->isEnd = true;
      return *this;
    }

    HashTableIterator operator++(int) {
      HashTableIterator result = *this;
      ++(*this);
      return result;
    }

    TYPE operator*() const {
      if (this->vecBegin == this->vecEnd) return nullptr;
      return *(this->vecBegin);
    }

    TYPE operator->() const {
      return this->operator*();
    }

    bool operator==(const HashTableIterator& rhs) const {
      return (this->isEnd && rhs.isEnd) || **this == *rhs;
    }

    bool operator!=(const HashTableIterator& rhs) const {
      return !(rhs == *this);
    }
  };
}

#endif //SLAM_HASHTABLEITERATOR_H
