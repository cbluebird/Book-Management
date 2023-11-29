//
// Created by 蔡瑞凯 on 2023/11/28.
//

#include "../../include/storage/index/node.h"

template<typename KeyType>
Node<KeyType>::Node(){
    key= new KeyType[MAX+1];
    ptr=new Node<KeyType>* [MAX+1];
    page_id=-1;
    size=0;
}