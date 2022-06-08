#ifndef SLAM_HASHTABLE_H
#define SLAM_HASHTABLE_H

#include "vector"
#include <iostream>
#include "TableEntry.h"
#include "HashTableIterator.h"
#include "strategies/HashStrategy.h"
#include "strategies/LinearHashStrategy.h"
#include "strategies/QuadraticHashStrategy.h"
#include "strategies/DoubleHashingStrategy.h"

namespace HashTable {
  template<typename T>
  class HashTable {
  private:
    HashStrategy<T>* strategy;
    std::vector<TableEntry<T>*> table;
    int nOccupied;

    [[nodiscard]] size_t tableSize() const {
      return this->table.size();
    }

    void moveIndexes() {
      for (size_t i = 0; i < this->tableSize(); ++i) {
        if (this->table.at(i) != nullptr && !this->table.at(i)->isDeleted()) {
          if (this->insert(this->table.at(i)->getValue())) {
            this->table.at(i)->setDeleted();
          }
        }
      }
    }

    void resize() {
      int newSize = this->tableSize() * 2;
      this->table.resize(newSize, nullptr);
      moveIndexes();
    }

//    size_t getLinearIndex(size_t index){
//      return this->indexFromHash(index + 1);
//    }
//
//    size_t getQuadraticIndex(size_t index, int nIters){
//      return this->indexFromHash(index + nIters*nIters);
//    }
//
//    size_t getDoubleHashingIndex(size_t hash, int nIters){
//      return this->indexFromHash(hash * (nIters + 1));
//    }

    std::pair<TableEntry<T>*, size_t> getFree(const T& toFind) {
      auto index = this->indexFromKey(toFind);
//      auto index = this->strategy->indexFromKey(toFind);

      TableEntry<T>* entry = table.at(index);
      int nIters = 0;
      while (entry != nullptr && !entry->isDeleted()) {
        if (entry->getValue() == toFind) return {entry, index};
//        index = this->getQuadraticIndex(index, nIters);
        index = this->indexFromHash(this->strategy->nextHash(index, 0 /*ignore*/ , nIters));
        entry = table.at(index);
        ++nIters;
      }
      return {entry, index};
    }

    std::pair<TableEntry<T>*, size_t> isPresent(const T& toFind) {
      auto index = this->indexFromKey(toFind);
//      auto index = this->strategy->indexFromKey(toFind);

      TableEntry<T>* entry = table.at(index);
      int nIters = 0;
      while (entry != nullptr) {
        if (entry->getValue() == toFind) return {entry, index};
//        index = this->getQuadraticIndex(index, nIters);
        index = this->indexFromHash(this->strategy->nextHash(index, 0 /*ignore*/ , nIters));
        entry = table.at(index);
        ++nIters;
      }
      return {entry, index};
    }

    std::pair<TableEntry<T>*, size_t> getEntry(const T& toFind, bool findFree) {
      if (findFree) return getFree(toFind);
      else return isPresent(toFind);
    }

    /**
     * Linear probing.
     * @param key
     * @return
     */
    bool insert(const T& key, bool isRealloc) {
      auto entryPair = getEntry(key, true);
      auto entry = entryPair.first;
      auto index = entryPair.second;

      if (entry == nullptr) {
        table[index] = new TableEntry<T>(key, key.hash());
      } else if (entry->isDeleted()) {
        entry->setValue(key);
      } else {
        // the element is already part of the set
        return false;
      }

      if (!isRealloc) ++nOccupied;
      if ((nOccupied * 100) / this->tableSize() > 75) resize();
      return true;
    }

  public:
    explicit HashTable(int size, HashStrategy<T>* strategy = new QuadraticHashStrategy<T>()) : nOccupied(0) {
      this->table.resize(size, nullptr);
      this->strategy = strategy;
    }

    HashTable() : HashTable(20) {}

    std::vector<TableEntry<T>*> getTable() const {
      return table;
    }

    [[nodiscard]] int size() const {
      return nOccupied;
    }

    void reserve(size_t newSize) {
      if (newSize < this->tableSize()) return;
      this->table.resize(newSize, nullptr);
    }

    size_t indexFromHash(const size_t i) {
      return i % this->tableSize();
    }

    size_t indexFromKey(const T& key) {
      return this->indexFromHash(key.hash());
    }

    bool contains(const T& toFind) {
      return this->getEntry(toFind, false).first != nullptr;
    }

    bool insert(const T& key) {
      return this->insert(key, false);
    }

    bool insert(const std::vector<T>& vec) {
      bool ret = true;
      for (const auto& elem: vec) {
        if (!insert(elem))
          ret = false;
      }
      return ret;
    }

    bool remove(const T& key) {
      auto entry = this->getEntry(key, false).first;
      if (entry != nullptr) {
        entry->setDeleted();
        --nOccupied;
        return true;
      }
      return false;
    }

    void merge(const HashTable& h) {
      for (TableEntry<T>* i: h.getAll()) {
        insert(i->getValue());
      }
    }

    std::vector<TableEntry<T>*> getAll() const {
      std::vector<TableEntry<T>*> ret;
      for (size_t i = 0; i < table.size(); i++) {
        if (table.at(i) != nullptr && !table.at(i)->isDeleted()) {
          ret.push_back(table.at(i));
        }
      }

      return ret;
    }

    typedef HashTableIterator<TableEntry<T>*> const_iterator;

    const_iterator begin() {
      return const_iterator(this->table);
    }

    const_iterator end() {
      return const_iterator(this->table, true);
    }
  };
}

#endif //SLAM_HASHTABLE_H