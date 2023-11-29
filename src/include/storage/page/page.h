//
// Created by 蔡瑞凯 on 2023/11/28.
//

#ifndef BOOK_MANAGEMENT_PAGE_H
#define BOOK_MANAGEMENT_PAGE_H

#include <unordered_map>

//一个node对应一个page，page作块状存储
template<typename ValueType,typename KeyType>
class Page {
public:
    int pin_count;
    bool is_dirty;
    int page_id;
    static int count;
    std::unordered_map<KeyType,ValueType> *data;
    Page();
    void deleteData(KeyType key);
    void Insert(ValueType value,KeyType key);
    ValueType getData(KeyType key);
    void updateData(ValueType,KeyType);
    int getPageId()const{return page_id;}
};

template<typename ValueType,typename KeyType>
int Page<ValueType,KeyType>::count = 1;

template<typename ValueType, typename KeyType>
Page<ValueType, KeyType>::Page() {
    page_id = -1;
    pin_count = 0;
    data =new std::unordered_map<KeyType,ValueType>;
}

template<typename ValueType, typename KeyType>
void Page<ValueType, KeyType>::Insert(ValueType value, KeyType key) {
    data->insert(std::make_pair(key,value));
}

template<typename ValueType, typename KeyType>
void Page<ValueType, KeyType>::deleteData(KeyType key) {
    data->erase(key);
}

template<typename ValueType, typename KeyType>
ValueType Page<ValueType, KeyType>::getData(KeyType key) {
    ValueType value = (*data)[key];
    return value;
}

template<typename ValueType, typename KeyType>
void Page<ValueType, KeyType>::updateData(ValueType value, KeyType key) {
    deleteData(key);
    Insert(value,key);
}

#endif //BOOK_MANAGEMENT_PAGE_H
