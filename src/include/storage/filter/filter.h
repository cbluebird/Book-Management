//
// Created by 蔡瑞凯 on 2023/11/28.
//

#ifndef BOOK_MANAGEMENT_FILTER_H
#define BOOK_MANAGEMENT_FILTER_H

#include <string>
#include<bitset>

class Filter {
    uint32_t murmur3(const std::string& key);
    std::bitset<10000> filter;
    std::hash<std::string> hashFunction;
public:
    void set(const std::string& element) ;
    bool get(const std::string& element) ;
    Filter()=default;
};


#endif //BOOK_MANAGEMENT_FILTER_H
