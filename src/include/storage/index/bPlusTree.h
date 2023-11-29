//
// Created by 蔡瑞凯 on 2023/11/28.
//

#ifndef BOOK_MANAGEMENT_BPLUSTREE_H
#define BOOK_MANAGEMENT_BPLUSTREE_H


#include <climits>
#include <fstream>
#include <iostream>
#include <sstream>
#include<vector>

#include "node.h"
#include "../buffer/bufferPool.h"
#include "../../../storage/buffer/bufferPool.cpp"
#include "../../../storage/index/node.cpp"

template<typename ValueType, typename KeyType>
class Iterator {
private:
    Node<KeyType>* m_start;//起始指向
    Node<KeyType>* m_cur;//现在指向
    Node<KeyType>* m_end;//中止指向
    int cur;
public:
    Node<KeyType>* getCur();
    Iterator<ValueType,KeyType>& operator=(const Iterator<ValueType,KeyType>&);
    Iterator<ValueType,KeyType> operator++(int);
    void gotoNextNode();
    Iterator();
    Iterator(Node<KeyType>*,Node<KeyType>*);
};


template<typename ValueType,typename KeyType>
class BPlusTree {
    Node<KeyType> *root;
    BufferPoolManager<ValueType,KeyType> *buffer_pool_manager;
    KeyType(*getMajor)(ValueType);

    friend class Iterator<ValueType,KeyType>;

    Node<KeyType> *begin() const;
    Node<KeyType> *end() const;
    void insertInternal(KeyType,Node<KeyType>*,Node<KeyType>*,Node<KeyType>*);
    void split(KeyType ,Node<KeyType> *,Node<KeyType> *,ValueType);
    int insertVal(KeyType ,Node<KeyType> *);
    void removeInternal(KeyType, Node<KeyType> *, Node<KeyType> *);
    Node<KeyType> *findParent(Node<KeyType> *, Node<KeyType> *);

public:
    BPlusTree();
    void insert(KeyType,ValueType);
    void remove(KeyType);
    void display();
    bool updateWithoutKey(KeyType,ValueType);
    bool updateKey(KeyType,KeyType,ValueType);
    ValueType* querySingleWithKey(const KeyType& ) const;
    ValueType* querySingleWithoutKey(const ValueType& ,bool(*)(const ValueType,const ValueType));
    std::vector<ValueType> queryAll();
    std::vector<ValueType> queryWithMatch(ValueType,bool(*)(ValueType,ValueType));
    Node<KeyType> *getRoot();
    KeyType getEndKey();
};





#endif //BOOK_MANAGEMENT_BPLUSTREE_H
