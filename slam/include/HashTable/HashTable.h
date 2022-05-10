#ifndef SLAM_HASHTABLE_H
#define SLAM_HASHTABLE_H

#include "vector"
#include <iostream>
#include "./TableEntry.h"



namespace HashTable {
  template<typename T>
  class HashTable {
  private:
    std::vector<TableEntry<T>*> table;
    int size;
    int nOccupied;

  public:
    HashTable() : HashTable(20) {}

    HashTable(int size) {
      this->size = size;
      this->table.resize(this->size, nullptr);
      nOccupied = 0;
    }

    size_t getHash(const T& key) {
      return key.hash() % this->size;
    }

    /**
     * Linear probing.
     * @param key
     * @return
     */
    bool insert(T key) {
      auto hash = this->getHash(key);

      TableEntry<T>* entry = table.at(hash);
      while (entry != nullptr && entry->getValue() != key) {
        if (entry->getDeleted())
          break;
        // go next
        hash = (hash + 1) % this->size;
        entry = table.at(hash);
      }

      if (table.at(hash) == nullptr) {
        table[hash] = new TableEntry<T>(key);
        nOccupied++;
      } else if (entry->getValue() == key) {
        // the element is already part of the set
        return false;
      } else {
        table.at(hash)->setValue(key);
        nOccupied++;
      }
      if((nOccupied*100)/size > 75) resize();
      return true;
    }

    bool erase(T key) {
      int hash = this->getHash(key);
      while (table.at(hash) != nullptr && table.at(hash)->getValue() != key) {
        hash += 1;
        hash %= this->size;
      }
      if (table.at(hash)->getValue() == key) {
        table.at(hash)->setDeleted();
        nOccupied--;
        return true;
      }
      return false;
    }

    void moveIndexes(){
      for(size_t i = 0; i < this->size; ++i){
        if(this->table.at(i) != nullptr && !this->table.at(i)->getDeleted()) {
          if(this->insert(this->table.at(i)->getValue())) {
            this->table.at(i)->setDeleted();
          }
        }
      }
    }

    void resize() {
      int newSize = this->size * this->size;
      this->table.resize(newSize);
      this->size = newSize;
      moveIndexes();
    }

    void printAll() {
      std::cout << "SIZE: " << this->size << std::endl;
      for (size_t i = 0; i < table.size(); i++) {
        if(table.at(i) != nullptr) {
          std::cout << "Index: " << i << std::endl;
          std::cout << "\t" << table.at(i)->getValue() << " - " << table.at(i)->getDeleted() << std::endl;
        }
      }
    }
  };
}


#endif //SLAM_HASHTABLE_H