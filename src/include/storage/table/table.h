//
// Created by 蔡瑞凯 on 2023/11/28.
//

#ifndef BOOK_MANAGEMENT_TABLE_H
#define BOOK_MANAGEMENT_TABLE_H



#include<shared_mutex>
#include<mutex>
#include<vector>
#include<thread>
#include <future>
#include "../index/bPlusTree.h"
#include "../disk/file.h"
#include "../filter/filter.h"

//主要是集合对应的功能，提供相应的接口
template<typename ValueType,typename KeyType>
class Table {
    BPlusTree<ValueType,KeyType>*bPlusTree;
    File<ValueType,KeyType>*file;
    Filter *filter;
    std::shared_mutex rwMutex;
public:
    Table(const std::string& path, const std::string& fileType, KeyType(*key)(ValueType));
    ~Table();
    KeyType(*getMajor)(ValueType);

    void insert(KeyType,ValueType);

    bool remove(KeyType);

    std::vector<ValueType>queryAll();
    std::vector<ValueType>queryWithMatch(ValueType,bool(*)(ValueType,ValueType));
    ValueType* querySingleWithKey(const KeyType& );
    ValueType* querySingleWithoutKey(const ValueType& ,bool(*)(const ValueType,const ValueType));
    KeyType getEndKey();

    void updateWithoutKey(ValueType&);
    void updateKey(KeyType,ValueType&);

    std::vector<ValueType> queryWithSort(bool (*cp)(ValueType, ValueType));
};




template<typename ValueType, typename KeyType>
Table<ValueType, KeyType>::~Table() {
    delete file;
    delete bPlusTree;
    delete filter;
}

template<typename ValueType, typename KeyType>
Table<ValueType, KeyType>::Table(const std::string &path, const std::string &fileType, KeyType(*key)(ValueType)) {
    filter =new Filter();
    file=new File<ValueType,KeyType>(path,fileType);
    bPlusTree=new BPlusTree<ValueType,KeyType>();
    std::unordered_map<KeyType,ValueType>temp=file->init(key);
    for(auto it=temp.begin();it!=temp.end();++it){
        filter->set(it->first);
        bPlusTree->insert(it->first,it->second);
    }
    getMajor=key;
}

template<typename ValueType, typename KeyType>
void Table<ValueType, KeyType>::insert(KeyType key, ValueType value) {
    std::unique_lock<std::shared_mutex> lock(rwMutex);
    std::async(std::launch::async, &BPlusTree<ValueType,KeyType>::insert,bPlusTree,key,value);
    std::async(std::launch::async, &File<ValueType,KeyType>::insert,file,value);
    std::async(std::launch::async,&Filter::set,filter,key);
}

template<typename ValueType, typename KeyType>
std::vector<ValueType> Table<ValueType, KeyType>::queryAll() {
    std::shared_lock<std::shared_mutex> lock(rwMutex);
    auto fu = std::async(std::launch::deferred, &BPlusTree<ValueType,KeyType>::queryAll,bPlusTree);
    return fu.get();
}

template<typename ValueType, typename KeyType>
bool Table<ValueType, KeyType>::remove(KeyType key) {
    std::unique_lock<std::shared_mutex> lock(rwMutex);
    if(!filter->get(key))return false;
    bPlusTree->remove(key);
    std::vector<ValueType>ans=bPlusTree->queryAll();
    file->update(ans);
    return true;
}


template<typename ValueType, typename KeyType>
ValueType *Table<ValueType, KeyType>::querySingleWithKey(const KeyType &key) {
    std::shared_lock<std::shared_mutex> lock(rwMutex);
    if(!filter->get(key))return nullptr;
    auto fu = std::async(std::launch::deferred, &BPlusTree<ValueType,KeyType>::querySingleWithKey,bPlusTree,key);
    return fu.get();
}

template<typename ValueType, typename KeyType>
ValueType *Table<ValueType, KeyType>::querySingleWithoutKey(const ValueType &value, bool (*key)(const ValueType, const ValueType))  {
    std::shared_lock<std::shared_mutex> lock(rwMutex);
    auto fu = std::async(std::launch::deferred, &BPlusTree<ValueType,KeyType>::querySingleWithoutKey,bPlusTree,value,key);
    return fu.get();
}

template<typename ValueType, typename KeyType>
void Table<ValueType, KeyType>::updateWithoutKey(ValueType& value) {
    std::unique_lock<std::shared_mutex> lock(rwMutex);
    auto key=getMajor(value);
    if(!filter->get(key))return;
    bPlusTree->updateWithoutKey(key, value);
    auto ans=bPlusTree->queryAll();
    file->update(ans);
}

template<typename ValueType, typename KeyType>
void Table<ValueType, KeyType>::updateKey(KeyType origin, ValueType& value) {
    std::unique_lock<std::shared_mutex> lock(rwMutex);
    auto newKey=getMajor(value);
    bPlusTree->updateKey(origin,newKey,value);
    auto ans=bPlusTree->queryAll();
    file->update(ans);
}

template<typename ValueType, typename KeyType>
std::vector<ValueType> Table<ValueType, KeyType>::queryWithMatch(ValueType value,bool (*compare)(ValueType,ValueType)) {
    std::shared_lock<std::shared_mutex> lock(rwMutex);
    auto fu = std::async(std::launch::deferred, &BPlusTree<ValueType,KeyType>::queryWithMatch,bPlusTree,value,compare);
    return fu.get();
}

template<typename ValueType, typename KeyType>
KeyType Table<ValueType, KeyType>::getEndKey() {
    return bPlusTree->getEndKey();
}

template<typename ValueType, typename KeyType>std::vector<ValueType> Table<ValueType, KeyType>::queryWithSort(bool(*cp)(ValueType value1,ValueType value2)) {
    std::shared_lock<std::shared_mutex> lock(rwMutex);
    auto ans=bPlusTree->queryAll();
    for(int i=0;i<ans.size()-1;i++) {
        int k=i;
        for(int j=i+1;j<ans.size();j++){
            if(cp(ans[j],ans[k]))
                k=j;
        }
        ValueType temp=ans[i];
        ans[i]=ans[k];
        ans[k]=temp;
    }
    return ans;
}


#endif //BOOK_MANAGEMENT_TABLE_H
