#ifndef __ITERATOR_H
#define __ITERATOR_H
#include <cassert>
#include "node.h"

/**
 * Skiplist const iterator
 */
template<class Key, class Value>
class Iterator {
private:
  Node<Key, Value> *pCurrent;

public:
  Iterator(Node<Key,Value> *p) : pCurrent(p) {}
  virtual ~Iterator() {}

  virtual const Key& key() const {
    assert(pCurrent != nullptr);
    return pCurrent->key();
  };

  virtual const Value& value() const {
    assert(pCurrent != nullptr);
    return pCurrent->value();
  };

  virtual const Value& operator*() {
    assert(pCurrent != nullptr);
    return pCurrent->value();
  };

  virtual const Value& operator->() {
    assert(pCurrent != nullptr);
    return pCurrent->value();
  };

  virtual bool operator==(const Iterator &b) const {
    return pCurrent == b.get();
  };

  virtual bool operator!=(const Iterator &d) const {
    return !operator==(d);
  };


  virtual Iterator& operator=(const Iterator &) {
    return *this;
  };

  virtual Iterator& operator++() {
    return *this;
  };

  virtual Iterator& operator++(int) {
    return *this;
  };

    const Node<Key, Value> *get() const{
        return pCurrent;
    };


};

#endif // __ITERATOR_H
