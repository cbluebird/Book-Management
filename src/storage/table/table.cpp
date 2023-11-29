//
// Created by 蔡瑞凯 on 2023/11/28.
//
#include <future>
#include <thread>
#include "../../include/storage/table/table.h"


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
        bPlusTree->insert(it->first,it->second);
    }
    getMajor=key;
}

template<typename ValueType, typename KeyType>
void Table<ValueType, KeyType>::insert(KeyType key, ValueType value) {
    std::unique_lock<std::shared_mutex> lock(rwMutex);
    std::async(std::launch::async, bPlusTree->insert(),key,value);
    std::async(std::launch::async, file->insert(),value);
    //std::async(std::launch::async, filter->set(),std::to_string(key));
//    bPlusTree->insert(key,value);
//    file->insert(value);
}

template<typename ValueType, typename KeyType>
std::vector<ValueType> Table<ValueType, KeyType>::queryAll() {
    std::shared_lock<std::shared_mutex> lock(rwMutex);
    std::future<std::vector<ValueType>> fu = std::async(std::launch::deferred, bPlusTree->queryAll());
//    std::vector<ValueType>ans=bPlusTree->queryAll();
    return fu.get();
}

template<typename ValueType, typename KeyType>
void Table<ValueType, KeyType>::remove(KeyType key) {
    std::unique_lock<std::shared_mutex> lock(rwMutex);
    bPlusTree->remove(key);
    //std::vector<ValueType>ans=bPlusTree->queryAll();
    file->update(bPlusTree->queryAll());
}


template<typename ValueType, typename KeyType>
ValueType *Table<ValueType, KeyType>::querySingleWithKey(const KeyType &key) {
    std::shared_lock<std::shared_mutex> lock(rwMutex);
    //auto ans=bPlusTree->querySingleWithKey(key);
    std::future<ValueType*> fu = std::async(std::launch::deferred, bPlusTree->querySingleWithKey,key);
    return fu.get();
}

template<typename ValueType, typename KeyType>
ValueType *Table<ValueType, KeyType>::querySingleWithoutKey(const ValueType &value, bool (*key)(const ValueType, const ValueType))  {
    std::shared_lock<std::shared_mutex> lock(rwMutex);
    std::future<ValueType*> fu = std::async(std::launch::deferred, bPlusTree->querySingleWithoutKey,value,key);
    //auto ans=bPlusTree->querySingleWithoutKey(value,key);
    return fu.get();
}

template<typename ValueType, typename KeyType>
void Table<ValueType, KeyType>::updateWithoutKey(ValueType& value) {
    std::unique_lock<std::shared_mutex> lock(rwMutex);
    auto key=getMajor(value);
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
    auto ans=bPlusTree->queryWithMatch(value,compare);
    return ans;
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
