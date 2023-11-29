//
// Created by 蔡瑞凯 on 2023/11/28.
//

#include "../../include/storage/index/bPlusTree.h"

template<typename ValueType, typename KeyType>
Iterator<ValueType, KeyType>::Iterator() {
    m_end= nullptr;
    m_start=nullptr;
    m_cur= nullptr;
    cur=0;
}

template<typename ValueType, typename KeyType>
Iterator<ValueType, KeyType> Iterator<ValueType, KeyType>::operator++(int) {
    Iterator<ValueType,KeyType> iterator= *this;
    if(iterator.m_cur->size==cur+1){
        m_cur=m_cur->ptr[0];
        cur=0;
    }else{
        cur++;
    }
    return iterator;
}

template<typename ValueType, typename KeyType>
Iterator<ValueType, KeyType>::Iterator(Node<KeyType>* begin,Node<KeyType>* end) {
    m_end= end;
    m_start=begin;
    m_cur= begin;
    cur=0;
}

template<typename ValueType, typename KeyType>
Iterator<ValueType, KeyType> &Iterator<ValueType, KeyType>::operator=(const Iterator<ValueType, KeyType> & iterator) {
    m_end= iterator.m_end;
    m_start=iterator.m_start;
    m_cur= iterator.m_cur;
    cur= iterator.cur;
    return *this;
}

template<typename ValueType, typename KeyType>
Node<KeyType> *Iterator<ValueType, KeyType>::getCur() {
    return m_cur;
}

template<typename ValueType, typename KeyType>
void Iterator<ValueType, KeyType>::gotoNextNode() {
    if(m_cur == m_end){
        m_cur= nullptr;
        return;
    }
    m_cur=m_cur->ptr[0];
}
