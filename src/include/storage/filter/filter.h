//
// Created by 蔡瑞凯 on 2023/11/28.
//

#ifndef BOOK_MANAGEMENT_FILTER_H
#define BOOK_MANAGEMENT_FILTER_H

#include <string>
#include<bitset>

class Filter {
    uint32_t murmur3(const void* key, size_t len, uint32_t seed );
    static const size_t FILTER_SIZE = 1000;
    std::bitset<FILTER_SIZE> filter;
    std::hash<std::string> hashFunction;
public:
    void set(int) ;
    bool get(int) ;
    Filter()=default;
};


#endif //BOOK_MANAGEMENT_FILTER_H
