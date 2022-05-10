#ifndef SLAM_HASHTABLE_H
#define SLAM_HASHTABLE_H

#include "vector"
#include <iostream>
#include "./TableEntry.h"

namespace HashTable{
  template<typename T>
  class HashTable{
  private:
    int size;
    std::vector<TableEntry<T>*> table;

  public:
    HashTable() : HashTable(20){}

    HashTable(int size){
      this->size = size;
      this->table.resize(this->size, nullptr);
    }

    int getHash(const T& key){
      return key.hash() % this->size;
    }

    bool insert(T key){
      int hash = this->getHash(key);
      while((table.at(hash) != nullptr && table.at(hash)->getValue() != key))
      {
        if(table.at(hash)->getDeleted())
          break;
        hash += 1;
        hash %= this->size;
      }
      if(table.at(hash)->getValue() == key){
        return false;
      }
      if(table.at(hash)->getDeleted())
        table.at(hash)->setValue(key);
      else
        table[hash] = new TableEntry<T>(key);
      std::cout << hash;
      return true;
    }

    bool erase(T key){
      int hash = this->getHash(key);
      while(table.at(hash) != nullptr && table.at(hash)->getValue() != key)
      {
        hash += 1;
        hash %= this->size;
      }
      if(table.at(hash)->getValue() == key){
        table.at(hash)->setDeleted();
        return false;
      }
      return false;
    }

    void printAll(){
      for(size_t i=0; i<table.size(); i++){
        std::cout << "Index: " << i << std::endl;
        std::cout << "\t" << table.at(i)->getValue() << " - " << table.at(i)->getDeleted();
      }
    }
  };
}


#endif //SLAM_HASHTABLE_H