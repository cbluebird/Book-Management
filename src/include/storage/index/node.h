//
// Created by 蔡瑞凯 on 2023/11/28.
//

#ifndef BOOK_MANAGEMENT_NODE_H
#define BOOK_MANAGEMENT_NODE_H


template<typename KeyType>
class Node {
public:
    bool IS_LEAF;
    int  size;
    Node<KeyType>** ptr;
    Node<KeyType>* parent;
    static const int MAX = 4;
    KeyType *key;
    int page_id;
public:
    Node();
};

template<typename KeyType>
Node<KeyType>::Node(){
    key= new KeyType[MAX+1];
    ptr=new Node<KeyType>* [MAX+1];
    page_id=-1;
    size=0;
}

#endif //BOOK_MANAGEMENT_NODE_H
