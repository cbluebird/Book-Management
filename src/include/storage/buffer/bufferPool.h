//
// Created by 蔡瑞凯 on 2023/11/28.
//

#ifndef BOOK_MANAGEMENT_BUFFERPOOL_H
#define BOOK_MANAGEMENT_BUFFERPOOL_H


#include <mutex>
#include <unordered_map>
#include <list>

#include "../page/page.h"
//#include "../../storage/page/page.cpp"


using page_id_type=int;
using frame_id_t=int;

//内部实现了一个LRU的replacer,同时对Page的一个并发控制
template<typename ValueType,typename KeyType>
class BufferPoolManager {
public:
    BufferPoolManager();
    ~BufferPoolManager(){delete []pages;};
   // KeyType(*getMajor)(ValueType);
    Page<ValueType,KeyType>* FetchPage(page_id_type page_id);
    bool DeletePage(page_id_type page_id);
    bool UnpinPage(page_id_type page_id, bool is_dirty);
    Page<ValueType,KeyType> *NewPage(page_id_type *page_id);

private:
    bool find_replace(frame_id_t *frame_id);
    std::mutex latch;
    const int capacity = 1000;
    std::list<frame_id_t> lru_list;
    std::unordered_map<frame_id_t, std::list<frame_id_t>::iterator> lruMap;
    std::list<frame_id_t>free_list;
    std::unordered_map<page_id_type,frame_id_t>page_table;
    Page<ValueType,KeyType> *pages;
    //DiskManager<ValueType,KeyType> *diskManager;

    bool Victim(frame_id_t *frame_id);
    void Pin(frame_id_t frame_id);
    void Unpin(frame_id_t frame_id);
};


#endif //BOOK_MANAGEMENT_BUFFERPOOL_H
