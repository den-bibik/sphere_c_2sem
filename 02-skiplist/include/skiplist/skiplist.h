#ifndef __SKIPLIST_H
#define __SKIPLIST_H
#include <functional>
#include "node.h"
#include "iterator.h"
#include <iostream>
#include <stdlib.h>


using namespace std;

/**
 * Skiplist interface
 */
template<class Key, class Value, size_t MAXHEIGHT, class Compare = std::less<Key>>
class SkipList {
private:
  Node<Key, Value> *pHead;
  Node<Key, Value> *pTail;

  IndexNode<Key, Value> *pTailIdx;
  IndexNode<Key, Value> *aHeadIdx[8];

    vector<IndexNode<Key, Value>*> nearest(Key k) const{
        vector<IndexNode<Key, Value>*> cur(MAXHEIGHT);
        for(int h = MAXHEIGHT - 1; h >= 0; h--){
            for(cur[h] = aHeadIdx[h];
                (cur[h] != pTailIdx) && (&(cur[h]->next()) != pTailIdx)&&(cur[h]->next().key() < k);
                cur[h] =  static_cast<IndexNode<Key, Value>*>(&cur[h]->next()))
            if(h - 1 >= 0) cur[h-1] = static_cast<IndexNode<Key, Value>*>(&cur[h]->down());
        }
        return cur;
    }






public:
  /**
   * Creates new empty skiplist
   */
  SkipList() {
      srand(time(NULL));
    pHead   = new DataNode<Key, Value>(nullptr, nullptr);
    pTail   = new DataNode<Key, Value>(nullptr, nullptr);

    Node<Key, Value> *prev = pHead;
    pTailIdx = new IndexNode<Key, Value>(pTail, pTail);
    for (int i=0; i < MAXHEIGHT; i++) {
      aHeadIdx[i] = new IndexNode<Key, Value>(prev, pHead);
      aHeadIdx[i]->next(pTailIdx);
      prev = aHeadIdx[i];
    }
  }

  /**
   * Disable copy constructor
   */
  SkipList(const SkipList& that) = delete;


  /**
   * Destructor
   */
  virtual ~SkipList() {
    delete pTailIdx;
    for (int i=0; i < MAXHEIGHT; i++) {
      delete aHeadIdx[i];
    }

    delete pHead;
    delete pTail;
  }

  /**
   * Assign new value for the key. If a such key already has
   * assosiation then old value returns, otherwise nullptr
   *
   * @param key key to be assigned with value
   * @param value to be added
   * @return old value for the given key or nullptr
   */
  void print() const{
      for(int h = MAXHEIGHT - 1; h >= 0; h--){
          cout << "level " << h << endl;
          for(Node<Key, Value> *cur = &aHeadIdx[h]->next(); cur != pTailIdx; cur = &cur->next()){
              cout << "Key " << cur->key() << " value " << cur->value() << endl;
          }
          cout << endl;
      }
  }

    virtual Value* Put(const Key& key, const Value& value) const {
      auto before = nearest(key);
      Node<Key, Value> *cur= new DataNode<Key, Value>(new Key(key), new Value(value));
      Node<Key, Value> *down = nullptr;
      int h = 0;
      do{
          IndexNode<Key, Value> * p = new IndexNode<Key, Value>(down, cur);
          p->next(dynamic_cast<IndexNode<Key,Value>*>(&(before[h]->next())));
          before[h]->next(p);
          down = dynamic_cast<Node<Key, Value>*>(p);
          h++;
      }while((h < MAXHEIGHT)&&(rand()%2 - 1));


    return nullptr;
  };

  /**
   * Put value only if there is no assosiation with key in
   * the list and returns nullptr
   *
   * If there is an established assosiation with the key already
   * method doesn't nothing and returns existing value
   *
   * @param key key to be assigned with value
   * @param value to be added
   * @return existing value for the given key or nullptr
   */
  virtual const Value* PutIfAbsent(const Key& key, const Value& value) {
      const Value *s = Get(key);
      if(s == nullptr) {
          Put(key, value);
          return nullptr;
      }
      return s;
  };

  /**
   * Returns value assigned for the given key or nullptr
   * if there is no established assosiation with the given key
   *
   * @param key to find
   * @return value assosiated with given key or nullptr
   */
  virtual const Value* Get(const Key& key) const {
    const Value* v = operator[](key);
      return v;
  };

  /**
   * Remove given key from the skpiplist and returns value
   * it has or nullptr in case if key wasn't assosiated with
   * any value
   *
   * @param key to be added
   * @return value for the removed key or nullptr
   */
  virtual Value* Delete(const Key& key) {
      auto cur = nearest(key);
      while(Get(key)) {
          for (int i = 0; i < MAXHEIGHT; i++) {
              IndexNode<Key, Value> *t = dynamic_cast<IndexNode<Key, Value> *>(&cur[i]->next());
              if ((t != pTailIdx) && (t->key() == key)) {
                  cur[i]->next(dynamic_cast<IndexNode<Key, Value> *>(&t->next()));
                  delete (t);
              }
          }
      }
    return nullptr;
  };

  /**
   * Same as Get
   */
  virtual const Value* operator[](const Key& key) const {
      IndexNode<Key, Value>* n = dynamic_cast<IndexNode<Key, Value>*>(&nearest(key)[0]->next());
      if(n != pTailIdx && n->key() == key)
          return &n->value();
    return nullptr;
  };

  /**
   * Return iterator onto very first key in the skiplist
   */
  virtual Iterator<Key, Value> cbegin(int test = 0) const {
      return Iterator<Key,Value>(&(aHeadIdx[0]->next()));
  };

  /**
   * Returns iterator to the first key that is greater or equals to
   * the given key
   */
  virtual Iterator<Key, Value> cfind(const Key &min) const {
    return Iterator<Key,Value>(pTailIdx);
  };

  /**
   * Returns iterator on the skiplist tail
   */
  virtual Iterator<Key, Value> cend() const {
    return Iterator<Key,Value>(pTailIdx);
  };
};
#endif // __SKIPLIST_H
