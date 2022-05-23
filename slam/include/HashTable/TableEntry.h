//
// Created by PC on 10/05/2022.
//

#ifndef SLAM_TABLEENTRY_H
#define SLAM_TABLEENTRY_H

template<typename T>
class TableEntry {
  private:
    bool deleted;
    T value;
    unsigned long hash;
  public:

  TableEntry(T value, unsigned long hash){
    this->value = value;
    this->deleted = false;
    this->hash = hash;
  }

  void setDeleted(){
    this->deleted = true;
  }

  bool isDeleted(){
    return this->deleted;
  }

  T getValue(){
    return this->value;
  }

  void setValue(T value){
    this->value = value;
  }

  unsigned long getHash(){
    return this->hash();
  }

};


#endif //SLAM_TABLEENTRY_H
