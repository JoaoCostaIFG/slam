#ifndef SLAM_HASHTABLE_H
#define SLAM_HASHTABLE_H

#include "vector"
#include <iostream>
#include "./TableEntry.h"
#include "./HashTableIterator.h"


namespace HashTable {
  template<typename T>
  class HashTable {
  private:
    std::vector<TableEntry<T>*> table;
    int nOccupied;

  public:
    std::vector<TableEntry<T>*> getTable() const {
      return table;
    }

    [[nodiscard]] int getSize() const {
      return this->table.size();
    }

    [[nodiscard]] int getNOccupied() const {
      return nOccupied;
    }

    explicit HashTable(int size) : nOccupied(0) {
      this->table.resize(size, nullptr);
    }

    HashTable() : HashTable(20) {}

    bool contains(T toFind) {
      auto index = this->getIndexFromHash(toFind);

      TableEntry<T>* entry = table.at(index);
      while (entry != nullptr) {
        if (entry->getValue() == toFind) return true;
        index = this->getIndex(index + 1);
        entry = table.at(index);
      }
      return false;
    }

    void insert(const std::vector<T>& vec) {
      for (const auto& elem: vec) {
        insert(elem);
      }
    }

    size_t getIndex(const size_t i) {
      return i % this->getSize();
    }

    size_t getIndexFromHash(const T& key) {
      return this->getIndex(key.hash());
    }

    void reserve(int newSize) {
      if (newSize < this->getSize()) return;
      this->table.resize(newSize, nullptr);
    }

    /**
     * Linear probing.
     * @param key
     * @return
     */
    bool insert(T key) {
      auto index = this->getIndexFromHash(key);
      TableEntry<T>* entry = table.at(index);
      while (entry != nullptr && entry->getValue() != key) {
        if (entry->getDeleted())
          break;
        // go next
        index = this->getIndex(index + 1);
        entry = table.at(index);
      }

      if (entry == nullptr) {
        table[index] = new TableEntry<T>(key, key.hash());
        nOccupied++;
      } else if (entry->getValue() == key) {
        // the element is already part of the set
        return false;
      } else {
        entry->setValue(key);
        nOccupied++;
      }
      if ((nOccupied * 100) / this->getSize() > 75) resize();
      return true;
    }

    /**
     * Linear probing.
     * @param key
     * @return
     */
    bool insert(T key, unsigned long hash) {
      auto index = this->getIndex(hash);

      TableEntry<T>* entry = table.at(index);
      while (entry != nullptr && entry->getValue() != key) {
        if (entry->getDeleted())
          break;
        // go next
        index = this->getIndex(index + 1);
        entry = table.at(index);
      }

      if (table.at(index) == nullptr) {
        table[index] = new TableEntry<T>(key, hash);
      } else if (entry->getValue() == key) {
        // the element is already part of the set
        return false;
      } else {
        table.at(index)->setValue(key);
      }
      return true;
    }

    bool remove(T key) {
      int hash = this->getIndexFromHash(key);
      while (table.at(hash) != nullptr && table.at(hash)->getValue() != key) {
        hash = this->getIndex(hash + 1);
      }
      if (table.at(hash)->getValue() == key) {
        table.at(hash)->setDeleted();
        nOccupied--;
        return true;
      }
      return false;
    }

    void moveIndexes() {
      for (size_t i = 0; i < this->getSize(); ++i) {
        if (this->table.at(i) != nullptr && !this->table.at(i)->getDeleted()) {
          if (this->insert(this->table.at(i)->getValue())) {
            this->table.at(i)->setDeleted();
          }
        }
      }
    }

    void resize() {
      int newSize = this->getSize() * 2;
      this->table.resize(newSize);
      moveIndexes();
    }

    void merge(HashTable& h) {
      for (TableEntry<T>* i: h.getAll()) {
        insert(i->getValue());
      }
    }

    std::vector<TableEntry<T>*> getAll() {
      std::vector<TableEntry<T>*> ret;
      for (size_t i = 0; i < table.size(); i++) {
        if (table.at(i) != nullptr && !table.at(i)->getDeleted()) {
          ret.push_back(table.at(i));
        }
      }
      return ret;
    }

    void printAll() {
      std::cout << "SIZE: " << this->getSize() << std::endl;
      for (size_t i = 0; i < table.size(); i++) {
        if (table.at(i) != nullptr) {
          std::cout << "Index: " << i << std::endl;
          std::cout << "\t" << table.at(i)->getValue() << " - " << table.at(i)->getDeleted() << std::endl;
        }
      }
    }

    HashTableIterator::HashTableIterator<TableEntry<T>*> getIterator(){
      return HashTableIterator::HashTableIterator<TableEntry<T>*>(&this->table);
    }
  };
}


#endif //SLAM_HASHTABLE_H