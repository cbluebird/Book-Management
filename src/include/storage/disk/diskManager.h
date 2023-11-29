//
// Created by 蔡瑞凯 on 2023/11/28.
//

#ifndef BOOK_MANAGEMENT_DISKMANAGER_H
#define BOOK_MANAGEMENT_DISKMANAGER_H


#include "file.h"
#include "../page/page.h"

template<typename ValueType,typename KeyType>
class DiskManager {
    //本地地址
    std::string path;
    std::string data;
    //通过page_id获取本地映射
    std::unordered_map<int,File<ValueType,KeyType>>page_table;
public:
    DiskManager(const std::string& path, const std::string& data);
    Page<ValueType,KeyType> getPageData(KeyType(*getMajor)(ValueType),int);
    bool writePage(Page<ValueType,KeyType> pages);
    bool newPage(int);
    bool deletePage(int);
};



#endif //BOOK_MANAGEMENT_DISKMANAGER_H
