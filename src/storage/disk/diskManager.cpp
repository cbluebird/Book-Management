//
// Created by 蔡瑞凯 on 2023/11/28.
//

#include "../../include/storage/disk/diskManager.h"


template<typename ValueType, typename KeyType>
DiskManager<ValueType, KeyType>::DiskManager(const std::string& path, const std::string& data) {
    std::ifstream in;
    in.open(data, std::ios::in);
    if(!in.is_open()) {
        std::ofstream out;
        out.open(data, std::ios::out | std::ios::app);
        out.close();
        in.open(data, std::ios::in);
    }
    while(!in.eof()) {
        int value;
        in >> value;
        page_table.insert(std::make_pair(value, File<ValueType,KeyType>(path+std::to_string(value),".txt")));
    }
    in.close();
}

template<typename ValueType, typename KeyType>
Page<ValueType,KeyType> DiskManager<ValueType, KeyType>::getPageData(KeyType (*getMajor)(ValueType),int page_id) {
    auto file=page_table[page_id];
    auto page=new Page<ValueType,KeyType>;
    page->page_id=page_id;
    page->data=file.init(getMajor);
    return page;
}

template<typename ValueType, typename KeyType>
bool DiskManager<ValueType, KeyType>::writePage(Page<ValueType,KeyType> pages) {
    auto file=page_table[pages.page_id];
    std::vector<ValueType> v;
    auto data_=*pages.data;
    for (const auto &p : data_){
        v.push_back(p.second);
    }
    file.update(v);
    return true;
}

template<typename ValueType, typename KeyType>
bool DiskManager<ValueType, KeyType>::newPage(int pages) {
    auto file=File<ValueType,KeyType>(path+std::to_string(pages),".txt");
    return true;
}

template<typename ValueType, typename KeyType>
bool DiskManager<ValueType, KeyType>::deletePage(int page_id) {
    remove((path+std::to_string(page_id)+".txt").c_str());
    page_table.erase(page_id);
    return true;
}


