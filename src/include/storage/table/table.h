//
// Created by 蔡瑞凯 on 2023/11/28.
//

#ifndef BOOK_MANAGEMENT_TABLE_H
#define BOOK_MANAGEMENT_TABLE_H



#include<shared_mutex>
#include<mutex>
#include<vector>
#include<thread>
#include "../index/bPlusTree.h"
#include "../disk/file.h"
#include "../../../storage/index/bPlusTree.cpp"
#include "../../../storage/disk/file.cpp"
#include "../filter/filter.h"

//主要是集合对应的功能，提供相应的接口
template<typename ValueType,typename KeyType>
class Table {
    BPlusTree<ValueType,KeyType>*bPlusTree;
    File<ValueType,KeyType>*file;
    Filter *filter;
    std::shared_mutex rwMutex;
public:
    Table(const std::string& path, const std::string& fileType, KeyType(*getMajor)(ValueType));
    ~Table();
    KeyType(*getMajor)(ValueType);

    void insert(KeyType,ValueType);

    void remove(KeyType);

    std::vector<ValueType>queryAll();
    std::vector<ValueType>queryWithMatch(ValueType,bool(*)(ValueType,ValueType));
    ValueType* querySingleWithKey(const KeyType& );
    ValueType* querySingleWithoutKey(const ValueType& ,bool(*)(const ValueType,const ValueType));
    KeyType getEndKey();

    void updateWithoutKey(ValueType&);
    void updateKey(KeyType,ValueType&);

    std::vector<ValueType> queryWithSort(bool (*cp)(ValueType, ValueType));
};


#endif //BOOK_MANAGEMENT_TABLE_H
