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
  TableEntry(T value, unsigned long hash) {
    this->value = value;
    this->deleted = false;
    this->hash = hash;
  }

  void setDeleted() {
    this->deleted = true;
  }

  bool isDeleted() const {
    return this->deleted;
  }

  T getValue() const {
    return this->value;
  }

  void setValue(T value, unsigned long hash) {
    this->value = value;
    this->hash = hash;
    this->deleted = false;
  }

  [[nodiscard]] unsigned long getHash() const {
    return this->hash();
  }

  T operator*() const {
    return this->value;
  }

  T operator->() const {
    return this->operator*();
  }
};


#endif //SLAM_TABLEENTRY_H
