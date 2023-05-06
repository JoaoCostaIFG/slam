#ifndef SLAM_HASHTABLE_H
#define SLAM_HASHTABLE_H

#include <iostream>
#include <vector>

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
    inline static float loadFactor = 0.75f;

    HashStrategy<T>* strategy;
    std::vector<TableEntry<T>*> table;
    int nOccupied;

    [[nodiscard]] size_t tableSize() const {
      return this->table.size();
    }

    void move(TableEntry<T>* entry) {
      const auto hash = entry->getHash();
      auto index = this->indexFromHash(hash);

      TableEntry<T>* search = table[index];
      size_t nIters = 1;
      // there are neither deleted entries nor repeated values
      while (search != nullptr) {
        // loop
        index = this->indexFromHash(hash + this->strategy->offset(hash, nIters++));
        search = table[index];
      }

      table[index] = entry;

      ++nOccupied;
    }

    void resize(size_t neededSize) {
      nOccupied = 0;

      auto oldTable = std::move(this->table);
      size_t newSize = oldTable.size() * 2;
      while (newSize < neededSize) newSize *= 2;
      this->table = std::vector<TableEntry<T>*>(oldTable.size() * 2, nullptr);

      for (size_t i = 0; i < oldTable.size(); ++i) {
        auto e = oldTable[i];
        if (e == nullptr) continue;
        if (e->isDeleted()) {
          delete e;
          continue;
        }
        this->move(e);
      }
    }

    bool moveInplace(TableEntry<T>* entry, size_t oldSize) {
      // if it doesn't fall on the first half of the set, just save it in a buffer
      size_t hash = entry->getHash();
      size_t index = this->indexFromHash(hash);

      // try to place it on the second half of the set
      TableEntry<T>* search = table[index];
      size_t nIters = 1;
      while (search != nullptr) {
        if (index < oldSize) return false;
        index = this->indexFromHash(hash + this->strategy->offset(hash, nIters++));
        search = table[index];
      }

      // check if it is possible to place it on the second half
      this->table[index] = entry;
      ++nOccupied;
      return true;
    }

    void resizeInplace(size_t neededSize) {
      nOccupied = 0;

      size_t oldSize = this->tableSize();

      size_t newSize = oldSize * 2;
      while (newSize < neededSize) newSize *= 2;
      this->reserveInner(newSize);

      std::vector<TableEntry<T>*> buffer;
      buffer.reserve(oldSize / 2);

      for (size_t i = 0; i < oldSize; ++i) {
        auto e = this->table[i];

        if (e == nullptr) continue;
        if (e->isDeleted()) {
          delete e;
          continue;
        }
        // if it doesn't fall on the first half of the set, just save it in a buffer
        if (!this->moveInplace(e, oldSize)) buffer.push_back(e);
        // clear first half
        this->table[i] = nullptr;
      }

      for (const auto& e: buffer) {
        this->move(e);
      }
    }

    TableEntry<T>* getEntry(const T& toFind) const {
      const auto hash = this->strategy->hash(toFind);
      auto index = this->indexFromHash(hash);

      TableEntry<T>* entry = table[index];
      size_t nIters = 1;
      while (entry != nullptr) {
        if (!entry->isDeleted() && entry->getValue() == toFind)
          return entry;
        index = this->indexFromHash(hash + this->strategy->offset(hash, nIters++));
        entry = table[index];
      }
      return nullptr;
    }

    void reserveInner(size_t newSize) {
      if (newSize < this->tableSize()) return;
      this->table.resize(newSize, nullptr);
    }

    static size_t nextPow2(const size_t x) {
      // x is power of 2
      if ((x & (x - 1)) == 0) return x;
      size_t ret = 1;
      while (ret < x) ret <<= 1;
      return ret;
    }

  public:
    // initial table size is kept as a power of 2, so the table size is always a power of 2
    // this enables quadratic probing and double hashing to work correctly
    explicit HashTable(size_t size = 32, HashStrategy<T>* strategy = new QuadraticHashStrategy<T>()) :
        table(nextPow2(size), nullptr),
        nOccupied(0) {
      this->strategy = strategy;
    }

    ~HashTable() {
      for (size_t i = 0; i < this->table.size(); ++i) {
        delete this->table[i];
      }
    }

    std::vector<TableEntry<T>*> getTable() const {
      return table;
    }

    [[nodiscard]] int size() const {
      return nOccupied;
    }

    size_t indexFromHash(const size_t i) const {
      return i % this->tableSize();
    }

    bool contains(const T& toFind) const {
      return this->getEntry(toFind) != nullptr;
    }

    size_t collisions = 0;

    /**
     * @param key
     * @return If container didn't "contain" the element
     */
    bool insert(const T& key) {
      const auto hash = this->strategy->hash(key);
      auto index = this->indexFromHash(hash);
      TableEntry<T>* entry = table[index];

      size_t nIters = 1;
      while (entry != nullptr) {
        ++collisions;
        if (entry->isDeleted()) {
          entry->setValue(key, hash);
          break;
        } else if (entry->getValue() == key) {
          return false;
        }
        // loop
        index = this->indexFromHash(hash + this->strategy->offset(hash, nIters++));
        entry = table[index];
      }

      // need to create the container
      if (entry == nullptr)
        table[index] = new TableEntry<T>(key, hash);

      // we pass 0 to the resize because we just want to double the current size (only 1 jump)
      if (++nOccupied > HashTable::loadFactor * this->tableSize()) resizeInplace(0);
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

    void merge(const HashTable& h, bool doReserve = true) {
      if (doReserve) {
        size_t maxNeededSize = nOccupied + h.size();
        if (maxNeededSize > HashTable::loadFactor * this->tableSize()) {
          this->resizeInplace(maxNeededSize);
        }
      }

      for (const TableEntry<T>* e: h) {
        insert(e->getValue());
      }
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

    void reserve(size_t newSize) {
      this->reserveInner(nextPow2(newSize));
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