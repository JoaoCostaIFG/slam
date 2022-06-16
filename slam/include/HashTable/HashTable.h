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

    void move(TableEntry<T>* entry) {
      const auto hash = entry->getHash();
      auto index = this->indexFromHash(hash);

      TableEntry<T>* search = table.at(index);
      int nIters = 1;
      // there are neither deleted entries nor repeated values
      while (search != nullptr) {
        // loop
        index = this->indexFromHash(hash + this->strategy->offset(hash, nIters++));
        search = table.at(index);
      }

      table[index] = entry;

      ++nOccupied;
    }

    void resize() {
      nOccupied = 0;

      auto oldTable = std::move(this->table);
      this->table = std::vector<TableEntry<T>*>(oldTable.size() * 2, nullptr);

      for (size_t i = 0; i < oldTable.size(); ++i) {
        auto e = oldTable.at(i);
        if (e == nullptr) continue;
        this->move(e);
      }
    }

    TableEntry<T>* getEntry(const T& toFind) const {
      const auto hash = this->strategy->hash(toFind);
      auto index = this->indexFromHash(hash);

      TableEntry<T>* entry = table.at(index);
      int nIters = 1;
      while (entry != nullptr) {
        if (!entry->isDeleted() && entry->getValue() == toFind)
          return entry;
        index = this->indexFromHash(hash + this->strategy->offset(hash, nIters++));
        entry = table.at(index);
      }
      return nullptr;
    }

  public:
    // PLEASE KEEP THE INITIAL TABLE SIZE A POWER OF 2, SO DOUBLE HASHING CAN WORK
    explicit HashTable(size_t size = 32, HashStrategy<T>* strategy = new QuadraticHashStrategy<T>()) :
        table(size, nullptr),
        nOccupied(0) {
      this->strategy = strategy;
    }

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

    size_t indexFromHash(const size_t i) const {
      return i % this->tableSize();
    }

    bool contains(const T& toFind) const {
      return this->getEntry(toFind) != nullptr;
    }

    unsigned int collisions = 0;

    /**
     * @param key
     * @return If container didn't "contain" the element
     */
    bool insert(const T& key) {
      const auto hash = this->strategy->hash(key);
      auto index = this->indexFromHash(hash);
      TableEntry<T>* entry = table.at(index);

      int nIters = 1;
      while (entry != nullptr) {
        if (entry->isDeleted()) {
          entry->setValue(key, hash);
          break;
        } else if (entry->getValue() == key) {
          return false;
        }
        ++collisions;
        // loop
        index = this->indexFromHash(hash + this->strategy->offset(hash, nIters++));
        entry = table.at(index);
      }

      // need to create the container
      if (entry == nullptr)
        table[index] = new TableEntry<T>(key, hash);

      ++nOccupied;
      if ((nOccupied * 100) / this->tableSize() > 75) resize();
      return true;
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
      auto entry = this->getEntry(key);
      if (entry != nullptr) {
        entry->setDeleted();
        --nOccupied;
        return true;
      }
      return false;
    }

    void merge(const HashTable& h) {
      for (const TableEntry<T>* e: h) {
        insert(e->getValue());
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

    const_iterator begin() const {
      return const_iterator(this->table);
    }

    const_iterator end() const {
      return const_iterator(this->table, true);
    }
  };
}

#endif //SLAM_HASHTABLE_H