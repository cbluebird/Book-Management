//
// Created by 蔡瑞凯 on 2023/11/28.
//

#include "../../include/storage/disk/file.h"

#include<iostream>

template<typename ValueType, typename KeyType>
std::unordered_map<KeyType, ValueType> File<ValueType, KeyType>::init(KeyType(*getMajor)(ValueType)) {
    std::unordered_map<KeyType,ValueType>temp;
    std::ifstream in;
    in.open(path + fileType, std::ios::in);
    if(!in.is_open()) {
        std::ofstream out;
        out.open(path + fileType, std::ios::out | std::ios::app);
        out.close();
        in.open(path + fileType, std::ios::in);
    }
    while(!in.eof()) {
        ValueType value;
        in >> value;
        temp.insert(std::make_pair(getMajor(value),value));
    }
    in.close();
    return temp;
}

template<typename ValueType, typename KeyType>
void File<ValueType, KeyType>::insert(ValueType value) {
    std::ofstream out;
    out.open(path + fileType, std::ios::app);
    if(!out.is_open()) {
        std::cout << "open error" << std::endl;
        return;
    }
    out << std::endl<<value;
    out.close();
}

template<typename ValueType, typename KeyType>
void File<ValueType, KeyType>::update(std::vector<ValueType>&v) {
    std::ofstream out;
    out.open(path + ".temp", std::ios::app);
    if(!out.is_open()) {
        std::cout << "open error" << std::endl;
        return;
    }
    for(int i=0;i<v.size();i++){
        out<<v[i]<<std::endl;
    }
    out.close();
    remove((path + fileType).c_str());
    rename((path + ".temp").c_str(), (path + fileType).c_str());
}

template<typename ValueType, typename KeyType>
std::unordered_map<KeyType,ValueType> File<ValueType, KeyType>::getData(){
    std::unordered_map<KeyType,ValueType>temp;
    std::ifstream in;
    in.open(path + fileType, std::ios::in);
    if(!in.is_open()) {
        std::ofstream out;
        out.open(path + fileType, std::ios::out | std::ios::app);
        out.close();
        in.open(path + fileType, std::ios::in);
    }
    while(!in.eof()) {
        ValueType value;
        in >> value;
        temp.insert(std::make_pair(getMajor(value),value));
    }
    in.close();
    return temp;

};