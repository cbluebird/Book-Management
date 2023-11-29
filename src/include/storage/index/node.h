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


#endif //BOOK_MANAGEMENT_NODE_H
