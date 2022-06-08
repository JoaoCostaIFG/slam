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
      nOccupied = 0;

      for (size_t i = 0; i < this->tableSize(); ++i) {
        if (this->table.at(i) == nullptr) continue;
        if (this->table.at(i)->isDeleted()) {
          delete this->table.at(i);
          this->table.at(i) = nullptr;
        } else {
          auto toMove = this->table.at(i)->getValue();
          delete this->table.at(i);
          this->table.at(i) = nullptr;
          this->insert(toMove);
        }
      }
    }

    void resize() {
      int newSize = this->tableSize() * 2;
      this->table.resize(newSize, nullptr);
      moveIndexes();
    }

    std::pair<TableEntry<T>*, size_t>
    getEntry(const T& toFind, bool (* pred)(const T&, const TableEntry<T>*)) const {
      const auto initialHash = toFind.hash();
      auto index = this->indexFromHash(initialHash);

      TableEntry<T>* entry = table.at(index);
      int nIters = 0;
      while (entry != nullptr) {
        if (pred(toFind, entry)) return {entry, index};
        index = this->indexFromHash(this->strategy->nextHash(index, initialHash, nIters++));
        entry = table.at(index);
      }
      return {nullptr, index};
    }

    static bool getEntryTest(const T& toFind, const TableEntry<T>* entry) {
      return !entry->isDeleted() && entry->getValue() == toFind;
    }

    std::pair<TableEntry<T>*, size_t> getEntry(const T& toFind) const {
      return this->getEntry(toFind, getEntryTest);
    }

    static bool getFreeTest(const T& toFind, const TableEntry<T>* entry) {
      return entry->isDeleted() || entry->getValue() == toFind;
    }

    std::pair<TableEntry<T>*, size_t> getFree(const T& toFind) const {
      return this->getEntry(toFind, getFreeTest);
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

    size_t indexFromHash(const size_t i) const {
      return i % this->tableSize();
    }

    size_t indexFromKey(const T& key) const {
      return this->indexFromHash(key.hash());
    }

    bool contains(const T& toFind) const {
      return this->getEntry(toFind).first != nullptr;
    }

    /**
     * @param key
     * @return
     */
    bool insert(const T& key) {
      auto entryPair = getFree(key);
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
      auto entry = this->getEntry(key).first;
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