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
    const std::vector<TableEntry<T>*> getTable() const {
      return table;
    }

    int getSize() const {
      return size;
    }

    int getNOccupied() const {
      return nOccupied;
    }

    HashTable() : HashTable(20) {}

    HashTable(int size) {
      this->size = size;
      this->table.resize(this->size, nullptr);
      nOccupied = 0;
    }

    bool contains(T toFind){
      auto index = this->getIndex(toFind);

      TableEntry<T>* entry = table.at(index);
      while (entry != nullptr) {
        if( entry->getValue() == toFind ) return true;
        index = (index + 1) % this->size;
        entry = table.at(index);
      }
      return false;
    }

    void insert(typename std::vector<T>::iterator itB, typename std::vector<T>::iterator itE){
      for(; itB <= itE; itB++){
        insert(*itB);
      }
    }

    size_t getIndex(const T& key) {
      return key.hash() % this->size;
    }

    void reserve(int newSize){
      this->table.resize(newSize, nullptr);
    }

    /**
     * Linear probing.
     * @param key
     * @return
     */
    bool insert(T key) {
      auto index = this->getIndex(key);

      TableEntry<T>* entry = table.at(index);
      while (entry != nullptr && entry->getValue() != key) {
        if (entry->getDeleted())
          break;
        // go next
        index = (index + 1) % this->size;
        entry = table.at(index);
      }

      if (table.at(index) == nullptr) {
        table[index] = new TableEntry<T>(key, key.hash());
        nOccupied++;
      } else if (entry->getValue() == key) {
        // the element is already part of the set
        return false;
      } else {
        table.at(index)->setValue(key);
        nOccupied++;
      }
      if((nOccupied*100)/size > 75) resize();
      return true;
    }

    /**
     * Linear probing.
     * @param key
     * @return
     */
    bool insert(T key, unsigned long hash) {
      auto index = hash % this->size;
      TableEntry<T>* entry = table.at(index);
      while (entry != nullptr && entry->getValue() != key) {
        if (entry->getDeleted())
          break;
        // go next
        index = (index + 1) % this->size;
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
      int hash = this->getIndex(key);
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
      int newSize = this->size * 2;
      this->table.resize(newSize);

      moveIndexes();
      this->size = newSize;
    }

    void merge(HashTable& h){
      for(TableEntry<T>* i : h.getAll()){
        insert(i->getValue());
      }
    }

    std::vector<TableEntry<T>*> getAll() {
      std::vector<TableEntry<T>*> ret;
      for (size_t i = 0; i < table.size(); i++) {
        if(table.at(i) != nullptr && !table.at(i)->getDeleted()) {
          ret.push_back(table.at(i));
        }
      }
      return ret;
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