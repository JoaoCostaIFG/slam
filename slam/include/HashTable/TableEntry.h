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
  public:

  TableEntry(T value){
    this->value = value;
    this->deleted = false;
  }

  void setDeleted(){
    this->deleted = true;
  }

  bool getDeleted(){
    return this->deleted;
  }

  T getValue(){
    return this->value;
  }

  void setValue(T value){
    this->value = value;
  }

};


#endif //SLAM_TABLEENTRY_H
