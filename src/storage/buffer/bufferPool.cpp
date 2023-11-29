//
// Created by 蔡瑞凯 on 2023/11/28.
//

#include "../../include/storage/buffer/bufferPool.h"

template<typename ValueType,typename KeyType>
bool BufferPoolManager<ValueType,KeyType>::Victim(frame_id_t *frame_id) {
    // 选择一个牺牲frame
    latch.lock();
    if (lruMap.empty()) {
        latch.unlock();
        return false;
    }

    // 选择列表尾部 也就是最少使用的frame
    frame_id_t lru_frame = lru_list.back();
    lruMap.erase(lru_frame);
    // 列表删除
    lru_list.pop_back();
    *frame_id = lru_frame;
    latch.unlock();
    return true;
}

template<typename ValueType,typename KeyType>
void BufferPoolManager<ValueType,KeyType>::Pin(frame_id_t frame_id) {
    // 被引用的frame 不能出现在lru list中

    if (lruMap.count(frame_id) != 0) {
        lru_list.erase(lruMap[frame_id]);
        lruMap.erase(frame_id);
    }

}

template<typename ValueType,typename KeyType>
void BufferPoolManager<ValueType,KeyType>::Unpin(frame_id_t frame_id) {
    // 加入lru list中
    //latch.lock();
    if (lruMap.count(frame_id) != 0) {
        latch.unlock();
        return;
    }
    // if list size >= capacity
    // while {delete front}
    while (lru_list.size() >= capacity) {
        frame_id_t need_del = lru_list.front();
        lru_list.pop_front();
        lruMap.erase(need_del);
    }
    // insert
    lru_list.push_front(frame_id);
    lruMap[frame_id] = lru_list.begin();
    //latch.unlock();
}

template<typename ValueType,typename KeyType>
BufferPoolManager<ValueType,KeyType>::BufferPoolManager() {
    pages=new Page<ValueType,KeyType>[capacity];
    for (int i = 0; i < capacity; i++) {
        free_list.push_back(i);
    }
    //diskManager=new DiskManager<ValueType,KeyType>;
}

template<typename ValueType,typename KeyType>
bool BufferPoolManager<ValueType,KeyType>::find_replace(frame_id_t *frame_id) {
    if (!free_list.empty()) {
        *frame_id = free_list.front();
        free_list.pop_front();
        return true;
    }
    //else we need to find a replace page
    if (Victim(frame_id)) {
        // Remove entry from page_table
        int replace_frame_id = -1;
        for (const auto &p : page_table) {
            page_id_type pid = p.first;
            frame_id_t fid = p.second;
            if (fid == *frame_id) {
                replace_frame_id = pid;
                break;
            }
        }
        if (replace_frame_id != -1) {
            Page<ValueType,KeyType> *replace_page = &pages[*frame_id];

            //If dirty, flush to disk
            if (replace_page->is_dirty) {
                //char *data = pages[page_table[replace_page->page_id]].data_;
                //diskManager->WritePage(replace_page->page_id, data);
                replace_page->pin_count=0 ;  // Reset pin_count
            }
            page_table[replace_page->page_id]=-1;
            //page_table.erase(replace_page->page_id);
        }

        return true;
    }

    return false;
}

template<typename ValueType,typename KeyType>
Page<ValueType,KeyType> *BufferPoolManager<ValueType,KeyType>::NewPage(page_id_type *page_id) {
    latch.lock();
    bool is_all = true;
    for (int i = 0; i < this->capacity; i++) {
        if (pages[i].pin_count == 0) {
            is_all = false;
            break;
        }
    }
    if (is_all) {
        latch.unlock();
        return nullptr;
    }
    // 2.
    frame_id_t victim_fid;
    if (!find_replace(&victim_fid)) {
        latch.unlock();
        return nullptr;
    }
    page_id_type new_page_id=Page<ValueType,KeyType>::count;
    Page<ValueType,KeyType>::count++;
    // 3.
    Page<ValueType,KeyType> *victim_page = &pages[victim_fid];
    victim_page->page_id = new_page_id;
    victim_page->pin_count++;
    Pin(victim_fid);
    page_table[new_page_id] = victim_fid;
    victim_page->is_dirty = false;
    *page_id = new_page_id;
    // [attention]
    // if this not write to disk directly
    // maybe meet below case:
    // 1. NewPage
    // 2. unpin(false)
    // 3. 由于其他页的操作导致该页被从buffer_pool中移除
    // 4. 这个时候在FetchPage， 就拿不到这个page了。
    // 所以这里先把它写回磁盘
//    disk_manager_->WritePage(victim_page->GetPageId(), victim_page->GetData());
    latch.unlock();
    return victim_page;
}

template<typename ValueType,typename KeyType>
Page<ValueType,KeyType> *BufferPoolManager<ValueType,KeyType>::FetchPage(page_id_type page_id) {
    latch.lock();
    if(page_id==-1){
        latch.unlock();
        return nullptr;
    }
    std::unordered_map<page_id_type, frame_id_t>::iterator it = page_table.find(page_id);
    // 1.1 P exists
    if (it != page_table.end()) {
        frame_id_t frame_id = it->second;
        Page<ValueType,KeyType> *page = &pages[frame_id];
        //
        page->pin_count++;        // pin the page
        Pin(frame_id);  // notify replacer

        latch.unlock();
        return page;
    }
    // 1.2 P not exist
    frame_id_t replace_fid;
    if (!find_replace(&replace_fid)) {
        latch.unlock();
        return nullptr;
    }
    Page<ValueType,KeyType> *replacePage = &pages[replace_fid];
    // 2. write it back to the disk
    if (replacePage->is_dirty) {
        //       disk_manager_->WritePage(replacePage->page_id_, replacePage->data_);
    }
    // 3
    page_table.erase(replacePage->page_id);
    // create new map
    // page_id <----> replaceFrameID;
    page_table[page_id] = replace_fid;
    // 4. update replacePage info
    Page<ValueType,KeyType> *newPage = replacePage;
    //   disk_manager_->ReadPage(page_id, newPage->data_);
    newPage->page_id = page_id;
    newPage->pin_count++;
    newPage->is_dirty= false;
    Pin(replace_fid);
    latch.unlock();

    return newPage;
}

template<typename ValueType,typename KeyType>
bool BufferPoolManager<ValueType,KeyType>::UnpinPage(page_id_type page_id, bool is_dirty) {
    latch.lock();
    // 1. 如果page_table中就没有
    auto iter = page_table.find(page_id);
    if (iter == page_table.end()) {
        latch.unlock();
        return false;
    }
    // 2. 找到要被unpin的page
    frame_id_t unpinned_Fid = iter->second;
    Page<ValueType,KeyType> *unpinned_page = &pages[unpinned_Fid];
    if (is_dirty) {
        unpinned_page->is_dirty = true;
    }
    // if page的pin_count == 0 则直接return
    if (unpinned_page->pin_count == 0) {
        latch.unlock();
        return false;
    }
    unpinned_page->pin_count--;
    if (unpinned_page->pin_count == 0) {
        Unpin(unpinned_Fid);
    }
    latch.unlock();
    return true;
}

template<typename ValueType, typename KeyType>
bool BufferPoolManager<ValueType, KeyType>::DeletePage(page_id_type page_id) {
    latch.lock();
    if(page_id==-1){
        latch.unlock();
        return false;
    }
    // 1.
    if (page_table.find(page_id) == page_table.end()) {
        latch.unlock();
        return true;
    }
    // 2.
    frame_id_t frame_id = page_table[page_id];
    Page<ValueType,KeyType> *page = &pages[frame_id];
    if (page->pin_count > 0) {
        latch.unlock();
        return false;
    }
    //diskManager->deletePage(page_id);
    // delete in disk in here
    page_table.erase(page_id);
    // reset metadata
    page->is_dirty = false;
    page->pin_count = 0;
    page->page_id = -1;
    // return it to the free list

    free_list.push_back(frame_id);
    latch.unlock();
    return true;
}
