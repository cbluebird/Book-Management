//
// Created by 蔡瑞凯 on 2023/11/28.
//

#ifndef BOOK_MANAGEMENT_BPLUSTREE_H
#define BOOK_MANAGEMENT_BPLUSTREE_H


#include <climits>
#include <fstream>
#include <iostream>
#include <sstream>
#include<queue>
#include<vector>

#include "node.h"
#include "../buffer/bufferPool.h"

template<typename ValueType, typename KeyType>
class Iterator {
private:
    Node<KeyType>* m_start;//起始指向
    Node<KeyType>* m_cur;//现在指向
    Node<KeyType>* m_end;//中止指向
    int cur;
public:
    Node<KeyType>* getCur();
    Iterator<ValueType,KeyType>& operator=(const Iterator<ValueType,KeyType>&);
    Iterator<ValueType,KeyType> operator++(int);
    void gotoNextNode();
    Iterator();
    Iterator(Node<KeyType>*,Node<KeyType>*);
};


template<typename ValueType,typename KeyType>
class BPlusTree {
    Node<KeyType> *root;
    BufferPoolManager<ValueType,KeyType> *buffer_pool_manager;
    KeyType(*getMajor)(ValueType);

    friend class Iterator<ValueType,KeyType>;

    Node<KeyType> *begin() const;
    Node<KeyType> *end() const;
    void insertInternal(KeyType,Node<KeyType>*,Node<KeyType>*,Node<KeyType>*);
    void split(KeyType ,Node<KeyType> *,Node<KeyType> *,ValueType);
    int insertVal(KeyType ,Node<KeyType> *);
    void removeInternal(KeyType, Node<KeyType> *, Node<KeyType> *);
    Node<KeyType> *findParent(Node<KeyType> *, Node<KeyType> *);

public:
    BPlusTree();
    void insert(KeyType ,ValueType);
    void remove(KeyType);
    void display();
    bool updateWithoutKey(KeyType,ValueType);
    bool updateKey(KeyType,KeyType,ValueType);
    ValueType* querySingleWithKey(const KeyType& ) const;
    ValueType* querySingleWithoutKey(const ValueType& ,bool(*)(const ValueType,const ValueType));
    std::vector<ValueType> queryAll();
    std::vector<ValueType> queryWithMatch(ValueType,bool(*)(ValueType,ValueType));
    Node<KeyType> *getRoot();
    KeyType getEndKey();
};


template<typename ValueType,typename KeyType>
void BPlusTree<ValueType,KeyType>::insert(KeyType keys,ValueType value) {
    if (root == nullptr) {
        root = new Node<KeyType>;
        root->key[0] = keys;
        root->IS_LEAF = true;
        root->size = 1;
        root->parent = nullptr;
        root->ptr[0]=root->ptr[1]= nullptr;
        int id;
        auto *page=buffer_pool_manager->NewPage(&id);
        root->page_id=id;
        page->Insert(value,keys);
        buffer_pool_manager->UnpinPage(id,true);
    } else {
        Node<KeyType> *cursor = root;
        Node<KeyType> *parent= nullptr;
        while (cursor->IS_LEAF == false) {
            parent = cursor;
            for (int i = 0; i < cursor->size; i++) {
                if (keys < cursor->key[i]) {
                    cursor = cursor->ptr[i];
                    break;
                }
                if (i == cursor->size - 1) {
                    cursor = cursor->ptr[i + 1];
                    break;
                }
            }
        }
        if (cursor->size < Node<KeyType>::MAX) {
            insertVal(keys,cursor);
            cursor->parent = parent;
            auto page=buffer_pool_manager->FetchPage(cursor->page_id);
            page->Insert(value,keys);
            buffer_pool_manager->UnpinPage(page->getPageId(),true);
        } else split(keys, parent, cursor,value);
    }
}

template<typename ValueType,typename KeyType>
int BPlusTree<ValueType,KeyType>::insertVal(KeyType x, Node<KeyType> *cursor) {
    int i = 0;
    while (x > cursor->key[i] && i < cursor->size) i++;
    for (int j = cursor->size; j > i; j--) cursor->key[j] = cursor->key[j - 1];
    cursor->key[i] = x;
    cursor->size++;
    return i;
}

template<typename ValueType,typename KeyType>
void BPlusTree<ValueType,KeyType>::split(KeyType x, Node<KeyType> *parent, Node<KeyType> *cursor,ValueType value) {
    Node<KeyType>* LLeaf=new Node<KeyType>;
    Node<KeyType>* RLeaf=new Node<KeyType>;
    insertVal(x,cursor);
    LLeaf->IS_LEAF=RLeaf->IS_LEAF=true;
    LLeaf->size=(Node<KeyType>::MAX+1)/2;
    RLeaf->size=(Node<KeyType>::MAX+1)-(Node<KeyType>::MAX+1)/2;
    LLeaf->ptr[0]=RLeaf;
    RLeaf->ptr[0]=cursor->ptr[0];
    LLeaf->ptr[1]=cursor->ptr[1];
    RLeaf->ptr[1]=LLeaf;
    if(LLeaf->ptr[1]!=nullptr){
        Node<KeyType>* temp=LLeaf->ptr[1];
        temp->ptr[0]=LLeaf;
    }
    auto page=buffer_pool_manager->FetchPage(cursor->page_id);
    page->Insert(value,x);
    int Lid,Rid;
    auto LPage=buffer_pool_manager->NewPage(&Lid);
    auto RPage=buffer_pool_manager->NewPage(&Rid);
    for (int i = 0;i < LLeaf->size; i++) {
        LLeaf->key[i]= cursor->key[i];
        LPage->Insert(page->getData(cursor->key[i]),cursor->key[i]);
    }
    for (int i = 0,j=LLeaf->size;i < RLeaf->size; i++,j++) {
        RLeaf->key[i]= cursor->key[j];
        RPage->Insert(page->getData(cursor->key[j]),cursor->key[j]);
    }
    RLeaf->page_id=Rid;
    LLeaf->page_id=Lid;
    buffer_pool_manager->UnpinPage(page->getPageId(),true);
    buffer_pool_manager->UnpinPage(LPage->getPageId(), true);
    buffer_pool_manager->UnpinPage(RPage->getPageId(), true);
    buffer_pool_manager->DeletePage(cursor->page_id);
    if(cursor==root){
        Node<KeyType>* newRoot=new Node<KeyType>;
        newRoot->key[0] = RLeaf->key[0];
        newRoot->ptr[0] = LLeaf;
        newRoot->ptr[1] = RLeaf;
        newRoot->IS_LEAF = false;
        newRoot->size = 1;
        root = newRoot;
        LLeaf->parent=RLeaf->parent=newRoot;
    }
    else {insertInternal(RLeaf->key[0],parent,LLeaf,RLeaf);}
}

template<typename ValueType,typename KeyType>
void BPlusTree<ValueType,KeyType>::insertInternal(KeyType x, Node<KeyType> *cursor, Node<KeyType> *LLeaf, Node<KeyType> *RRLeaf) {
    {
        if (cursor->size < Node<KeyType>::MAX) {
            auto i=insertVal(x,cursor);
            for (int j = cursor->size;j > i + 1; j--) {
                cursor->ptr[j]= cursor->ptr[j - 1];
            }
            cursor->ptr[i]=LLeaf;
            cursor->ptr[i + 1] = RRLeaf;
        }

        else {

            Node<KeyType>* newLchild = new Node<KeyType>;
            Node<KeyType>* newRchild = new Node<KeyType>;
            Node<KeyType>* virtualPtr[Node<KeyType>::MAX + 2];
            for (int i = 0; i < Node<KeyType>::MAX + 1; i++) {
                virtualPtr[i] = cursor->ptr[i];
            }
            int i=insertVal(x,cursor);
            for (int j = Node<KeyType>::MAX + 1;j > i + 1; j--) {
                virtualPtr[j]= virtualPtr[j - 1];
            }
            virtualPtr[i]=LLeaf;
            virtualPtr[i + 1] = RRLeaf;
            newLchild->IS_LEAF=newRchild->IS_LEAF = false;
            //这里和叶子结点上有区别的
            newLchild->size= (Node<KeyType>::MAX + 1) / 2;
            newRchild->size= Node<KeyType>::MAX - (Node<KeyType>::MAX + 1) /2;
            for (int i = 0;i < newLchild->size;i++) {
                newLchild->key[i]= cursor->key[i];
            }
            for (int i = 0, j = newLchild->size+1;i < newRchild->size;i++, j++) {

                newRchild->key[i]= cursor->key[j];
            }
            for (int i = 0;i < LLeaf->size + 1;i++) {
                newLchild->ptr[i]= virtualPtr[i];
            }
            for (int i = 0, j = LLeaf->size + 1;i < RRLeaf->size + 1;i++, j++) {
                newRchild->ptr[i]= virtualPtr[j];
            }
            if (cursor == root) {
                Node<KeyType>* newRoot = new Node<KeyType>;
                newRoot->key[0]= cursor->key[newLchild->size];
                newRoot->ptr[0] = newLchild;
                newRoot->ptr[1] = newRchild;
                newRoot->IS_LEAF = false;
                newRoot->size = 1;
                root = newRoot;
                newLchild->parent=newRchild->parent=newRoot;
            }
            else {
                insertInternal(cursor->key[newLchild->size],cursor->parent,newLchild,newRchild);
            }
        }
    }
}

template<typename ValueType,typename KeyType>
void BPlusTree<ValueType,KeyType>::display() {
    std::queue<Node<KeyType> *> q;
    q.push(root);
    while (!q.empty()) {
        int size_t = q.size();
        while (size_t--) {
            auto t = q.front();
            for (int i = 0; i < t->size + 1; i++) {
                if (!t->IS_LEAF) {
                    q.push(t->ptr[i]);
                }
            }
            for (int i = 0; i < t->size; i++) {
                std::cout << t->key[i] << ",";
            }
            std::cout << "  ";
            q.pop();
        }
        std::cout << std::endl;
    }
}

template<typename ValueType, typename KeyType>
BPlusTree<ValueType, KeyType>::BPlusTree() {
    root = nullptr;
    buffer_pool_manager = new BufferPoolManager<ValueType,KeyType>();
}

template<typename ValueType, typename KeyType>
void BPlusTree<ValueType, KeyType>::remove(KeyType x) {
    if (root == nullptr) {
        std::cout << "Tree empty\n";
    } else {
        Node<KeyType> *cursor = root;
        Node<KeyType> *parent;
        int leftSibling, rightSibling;
        while (cursor->IS_LEAF == false) {
            for (int i = 0; i < cursor->size; i++) {
                parent = cursor;
                leftSibling = i - 1;
                rightSibling = i + 1;
                if (x < cursor->key[i]) {
                    cursor = cursor->ptr[i];
                    break;
                }
                if (i == cursor->size - 1) {
                    leftSibling = i;
                    rightSibling = i + 2;
                    cursor = cursor->ptr[i + 1];
                    break;
                }
            }
        }
        bool found = false;
        int pos;
        for (pos = 0; pos < cursor->size; pos++) {
            if (cursor->key[pos] == x) {
                found = true;
                break;
            }
        }
        if (!found) {
            std::cout << "Not found\n";
            return;
        }
        for (int i = pos; i < cursor->size; i++) {
            cursor->key[i] = cursor->key[i + 1];
        }
        cursor->size--;
        auto page=buffer_pool_manager->FetchPage(cursor->page_id);
        page->deleteData(x);
        buffer_pool_manager->UnpinPage(cursor->page_id,true);
        if (cursor == root) {
            for (int i = 0; i < Node<KeyType>::MAX + 1; i++) {
                cursor->ptr[i] = nullptr;
            }
            if (cursor->size == 0) {
                std::cout << "Tree died\n";
                delete[] cursor->key;
                delete[] cursor->ptr;
                delete cursor;
                buffer_pool_manager->DeletePage(cursor->page_id);
                root = nullptr;
            }
            return;
        }
//        cursor->ptr[cursor->size] = cursor->ptr[cursor->size + 1];
//        cursor->ptr[cursor->size + 1] = nullptr;
        if (cursor->size >= (Node<KeyType>::MAX + 1) / 2) {
            return;
        }
        if (leftSibling >= 0) {
            Node<KeyType> *leftNode = parent->ptr[leftSibling];
            if (leftNode->size >= (Node<KeyType>::MAX + 1) / 2 + 1) {
                for (int i = cursor->size; i > 0; i--) {
                    cursor->key[i] = cursor->key[i - 1];
                }
                cursor->size++;
//                cursor->ptr[cursor->size] = cursor->ptr[cursor->size - 1];
//                cursor->ptr[cursor->size - 1] = NULL;
                cursor->key[0] = leftNode->key[leftNode->size - 1];
                auto lPage=buffer_pool_manager->FetchPage(leftNode->page_id);
                auto cPage=buffer_pool_manager->FetchPage(cursor->page_id);
                cPage->Insert(lPage->getData(cursor->key[0]),cursor->key[0]);
                lPage->deleteData(cursor->key[0]);
                buffer_pool_manager->UnpinPage(leftNode->page_id,true);
                buffer_pool_manager->UnpinPage(cursor->page_id,true);
                leftNode->size--;
//                leftNode->ptr[leftNode->size] = cursor;
//                leftNode->ptr[leftNode->size + 1] = NULL;
                leftNode->ptr[0] = cursor;
                parent->key[leftSibling] = cursor->key[0];
                return;
            }
        }
        if (rightSibling <= parent->size) {
            Node<KeyType> *rightNode = parent->ptr[rightSibling];
            if (rightNode->size >= (Node<KeyType>::MAX + 1) / 2 + 1) {
                cursor->size++;
//                cursor->ptr[cursor->size] = cursor->ptr[cursor->size - 1];
//                cursor->ptr[cursor->size - 1] = NULL;
                cursor->key[cursor->size - 1] = rightNode->key[0];
                rightNode->size--;
//                rightNode->ptr[rightNode->size] = rightNode->ptr[rightNode->size + 1];
//                rightNode->ptr[rightNode->size + 1] = NULL;
                for (int i = 0; i < rightNode->size; i++) {
                    rightNode->key[i] = rightNode->key[i + 1];
                }
                auto rPage=buffer_pool_manager->FetchPage(rightNode->page_id);
                auto cPage=buffer_pool_manager->FetchPage(cursor->page_id);
                cPage->Insert(rPage->getData(cursor->key[cursor->size-1]),cursor->key[cursor->size-1]);
                rPage->deleteData(cursor->key[cursor->size-1]);
                buffer_pool_manager->UnpinPage(rightNode->page_id,true);
                buffer_pool_manager->UnpinPage(cursor->page_id,true);
                parent->key[rightSibling - 1] = rightNode->key[0];
                return;
            }
        }
        if (leftSibling >= 0) {
            Node<KeyType> *leftNode = parent->ptr[leftSibling];
            auto lPage=buffer_pool_manager->FetchPage(leftNode->page_id);
            auto cPage=buffer_pool_manager->FetchPage(cursor->page_id);
            for (int i = leftNode->size, j = 0; j < cursor->size; i++, j++) {
                lPage->Insert(cPage->getData(cursor->key[j]),cursor->key[j]);
                cPage->deleteData(cursor->key[j]);
                leftNode->key[i] = cursor->key[j];
            }
            buffer_pool_manager->UnpinPage(leftNode->page_id,true);
            buffer_pool_manager->UnpinPage(cursor->page_id, true);
            //leftNode->ptr[leftNode->size] = NULL;
            leftNode->size += cursor->size;
//            leftNode->ptr[leftNode->size] = cursor->ptr[cursor->size];
            leftNode->ptr[0] = cursor->ptr[0];
            removeInternal(parent->key[leftSibling], parent, cursor);
            delete[] cursor->key;
            delete[] cursor->ptr;
            delete cursor;
            buffer_pool_manager->DeletePage(cursor->page_id);
        } else if (rightSibling <= parent->size) {
            Node<KeyType> *rightNode = parent->ptr[rightSibling];
            auto rPage=buffer_pool_manager->FetchPage(rightNode->page_id);
            auto cPage=buffer_pool_manager->FetchPage(cursor->page_id);
            for (int i = cursor->size, j = 0; j < rightNode->size; i++, j++) {
                cPage->Insert(rPage->getData(rightNode->key[j]),rightNode->key[j]);
                rPage->deleteData(rightNode->key[j]);
                cursor->key[i] = rightNode->key[j];
            }
            buffer_pool_manager->UnpinPage(rightNode->page_id,true);
            buffer_pool_manager->UnpinPage(cursor->page_id,true);
            //cursor->ptr[cursor->size] = NULL;
            cursor->size += rightNode->size;
            //cursor->ptr[cursor->size] = rightNode->ptr[rightNode->size];
            cursor->ptr[0] = rightNode->ptr[0];
            std::cout << "Merging two leaf nodes\n";
            removeInternal(parent->key[rightSibling - 1], parent, rightNode);
            delete[] rightNode->key;
            delete[] rightNode->ptr;
            delete rightNode;
            buffer_pool_manager->DeletePage(rightNode->page_id);
        }
    }
}

template<typename ValueType, typename KeyType>
void BPlusTree<ValueType, KeyType>::removeInternal(KeyType x, Node<KeyType> *cursor, Node<KeyType> *child) {
    if (cursor == root) {
        if (cursor->size == 1) {
            if (cursor->ptr[1] == child) {
                delete[] child->key;
                delete[] child->ptr;
                delete child;
                root = cursor->ptr[0];
                delete[] cursor->key;
                delete[] cursor->ptr;
                delete cursor;
                std::cout << "Changed root node\n";
                return;
            } else if (cursor->ptr[0] == child) {
                delete[] child->key;
                delete[] child->ptr;
                delete child;
                root = cursor->ptr[1];
                delete[] cursor->key;
                delete[] cursor->ptr;
                delete cursor;
                std::cout << "Changed root node\n";
                return;
            }
        }
    }
    int pos;
    for (pos = 0; pos < cursor->size; pos++) {
        if (cursor->key[pos] == x) {
            break;
        }
    }
    for (int i = pos; i < cursor->size; i++) {
        cursor->key[i] = cursor->key[i + 1];
    }
    for (pos = 0; pos < cursor->size + 1; pos++) {
        if (cursor->ptr[pos] == child) {
            break;
        }
    }
    for (int i = pos; i < cursor->size + 1; i++) {
        cursor->ptr[i] = cursor->ptr[i + 1];
    }
    cursor->size--;
    if (cursor->size >= (Node<KeyType>::MAX + 1) / 2 - 1) {
        return;
    }
    if (cursor == root) return;
    Node<KeyType> *parent = findParent(root, cursor);
    int leftSibling, rightSibling;
    for (pos = 0; pos < parent->size + 1; pos++) {
        if (parent->ptr[pos] == cursor) {
            leftSibling = pos - 1;
            rightSibling = pos + 1;
            break;
        }
    }
    if (leftSibling >= 0) {
        Node<KeyType> *leftNode = parent->ptr[leftSibling];
        if (leftNode->size >= (Node<KeyType>::MAX + 1) / 2) {
            for (int i = cursor->size; i > 0; i--) {
                cursor->key[i] = cursor->key[i - 1];
            }
            cursor->key[0] = parent->key[leftSibling];
            parent->key[leftSibling] = leftNode->key[leftNode->size - 1];
            for (int i = cursor->size + 1; i > 0; i--) {
                cursor->ptr[i] = cursor->ptr[i - 1];
            }
            cursor->ptr[0] = leftNode->ptr[leftNode->size];
            cursor->size++;
            leftNode->size--;
            return;
        }
    }
    if (rightSibling <= parent->size) {
        Node<KeyType> *rightNode = parent->ptr[rightSibling];
        if (rightNode->size >= (Node<KeyType>::MAX + 1) / 2) {
            cursor->key[cursor->size] = parent->key[pos];
            parent->key[pos] = rightNode->key[0];
            for (int i = 0; i < rightNode->size - 1; i++) {
                rightNode->key[i] = rightNode->key[i + 1];
            }
            cursor->ptr[cursor->size + 1] = rightNode->ptr[0];
            for (int i = 0; i < rightNode->size; ++i) {
                rightNode->ptr[i] = rightNode->ptr[i + 1];
            }
            cursor->size++;
            rightNode->size--;
            return;
        }
    }
    if (leftSibling >= 0) {
        Node<KeyType> *leftNode = parent->ptr[leftSibling];
        leftNode->key[leftNode->size] = parent->key[leftSibling];
        for (int i = leftNode->size + 1, j = 0; j < cursor->size; j++) {
            leftNode->key[i] = cursor->key[j];
        }
        for (int i = leftNode->size + 1, j = 0; j < cursor->size + 1; j++) {
            leftNode->ptr[i] = cursor->ptr[j];
            cursor->ptr[j] = NULL;
        }
        leftNode->size += cursor->size + 1;
        cursor->size = 0;
        removeInternal(parent->key[leftSibling], parent, cursor);
    } else if (rightSibling <= parent->size) {
        Node<KeyType> *rightNode = parent->ptr[rightSibling];
        cursor->key[cursor->size] = parent->key[rightSibling - 1];
        for (int i = cursor->size + 1, j = 0; j < rightNode->size; j++) {
            cursor->key[i] = rightNode->key[j];
        }
        for (int i = cursor->size + 1, j = 0; j < rightNode->size + 1; j++) {
            cursor->ptr[i] = rightNode->ptr[j];
            rightNode->ptr[j] = NULL;
        }
        cursor->size += rightNode->size + 1;
        rightNode->size = 0;
        removeInternal(parent->key[rightSibling - 1], parent, rightNode);
    }
}

template<typename ValueType, typename KeyType>
Node<KeyType> *BPlusTree<ValueType, KeyType>::findParent(Node<KeyType> *cursor, Node<KeyType> *child) {
    Node<KeyType> *parent;
    if (cursor->IS_LEAF || (cursor->ptr[0])->IS_LEAF) {
        return NULL;
    }
    for (int i = 0; i < cursor->size + 1; i++) {
        if (cursor->ptr[i] == child) {
            parent = cursor;
            return parent;
        } else {
            parent = findParent(cursor->ptr[i], child);
            if (parent != NULL) return parent;
        }
    }
    return parent;
}

template<typename ValueType, typename KeyType>
Node<KeyType> *BPlusTree<ValueType, KeyType>::begin() const{
    if(root== nullptr)return nullptr;
    Node<KeyType> *node= root;
    while(node->IS_LEAF== false) {
        node=node->ptr[0];
    }
    return node;
}

template<typename ValueType, typename KeyType>
Node<KeyType> *BPlusTree<ValueType, KeyType>::end() const{
    if(root== nullptr)return nullptr;
    Node<KeyType> *node= root;
    while(node->IS_LEAF== false) {
        node=node->ptr[node->size];
    }
    return node;
}

template<typename ValueType, typename KeyType>
std::vector<ValueType> BPlusTree<ValueType, KeyType>::queryAll()  {
    std::vector<ValueType>answer;
    if(root== nullptr)return answer;
    Iterator<ValueType,KeyType>iterator= Iterator<ValueType,KeyType>(begin(),end());
    while(iterator.getCur()!= nullptr) {
        Node<KeyType>* node = iterator.getCur();
        Page<ValueType,KeyType>* page=buffer_pool_manager->FetchPage(node->page_id);
        for (int i = 0; i < node->size; i++) {
            answer.push_back(page->getData(node->key[i]));
        }
        buffer_pool_manager->UnpinPage(node->page_id, false);
        iterator.gotoNextNode();
    }
    return answer;
}

template<typename ValueType, typename KeyType>
ValueType* BPlusTree<ValueType, KeyType>::querySingleWithKey(const KeyType & x) const {
    if(root== nullptr)return nullptr;
    Node<KeyType> *cursor = root;
    Node<KeyType> *parent;
    int leftSibling, rightSibling;
    while (cursor->IS_LEAF == false) {
        for (int i = 0; i < cursor->size; i++) {
            parent = cursor;
            leftSibling = i - 1;
            rightSibling = i + 1;
            if (x < cursor->key[i]) {
                cursor = cursor->ptr[i];
                break;
            }
            if (i == cursor->size - 1) {
                leftSibling = i;
                rightSibling = i + 2;
                cursor = cursor->ptr[i + 1];
                break;
            }
        }
    }
    bool found = false;
    int pos;
    for (pos = 0; pos < cursor->size; pos++) {
        if (cursor->key[pos] == x) {
            found = true;
            break;
        }
    }
    if (!found) {
        std::cout << "Not found\n";
        return nullptr;
    }
    Page<ValueType,KeyType> *page=buffer_pool_manager->FetchPage(cursor->page_id);
    auto *value= new ValueType;
    *value=page->getData(x);
    buffer_pool_manager->UnpinPage(cursor->page_id, false);
    return value;
}

template<typename ValueType, typename KeyType>
ValueType* BPlusTree<ValueType, KeyType>::querySingleWithoutKey(const ValueType & value,bool(*compare)(const ValueType,const ValueType))  {
    if(root== nullptr)return nullptr;
    Iterator<ValueType,KeyType>iterator=Iterator<ValueType,KeyType>(begin(),end());
    while(iterator.getCur()!= nullptr) {
        auto node=iterator.getCur();
        auto *page=buffer_pool_manager->FetchPage(node->page_id);
        for(int i=0;i<node->size;i++) {
            auto temp=new ValueType;
            *temp=page->getData(node->key[i]);
            if(compare(*temp,value)){
                return temp;
            }
        }
        buffer_pool_manager->UnpinPage(node->page_id,false);
        iterator.gotoNextNode();
    }
    return nullptr;
}

template<typename ValueType, typename KeyType>
Node<KeyType> *BPlusTree<ValueType, KeyType>::getRoot() {
    return root;
}

template<typename ValueType, typename KeyType>
bool BPlusTree<ValueType, KeyType>::updateWithoutKey(KeyType x, ValueType value) {
    if(root== nullptr)return false;
    Node<KeyType> *cursor = root;
    Node<KeyType> *parent;
    int leftSibling, rightSibling;
    while (cursor->IS_LEAF == false) {
        for (int i = 0; i < cursor->size; i++) {
            parent = cursor;
            leftSibling = i - 1;
            rightSibling = i + 1;
            if (x < cursor->key[i]) {
                cursor = cursor->ptr[i];
                break;
            }
            if (i == cursor->size - 1) {
                leftSibling = i;
                rightSibling = i + 2;
                cursor = cursor->ptr[i + 1];
                break;
            }
        }
    }
    bool found = false;
    int pos;
    for (pos = 0; pos < cursor->size; pos++) {
        if (cursor->key[pos] == x) {
            found = true;
            break;
        }
    }
    if (!found) {
        std::cout << "Not found\n";
        return false;
    }
    auto *page=buffer_pool_manager->FetchPage(cursor->page_id);
    page->updateData(value,x);
    buffer_pool_manager->UnpinPage(cursor->page_id,true);
    return true;
}

template<typename ValueType, typename KeyType>
bool BPlusTree<ValueType, KeyType>::updateKey(KeyType origin, KeyType newKey, ValueType value) {
    if(root== nullptr)return false;
    Node<KeyType> *cursor = root;
    Node<KeyType> *parent;
    int leftSibling, rightSibling;
    while (cursor->IS_LEAF == false) {
        for (int i = 0; i < cursor->size; i++) {
            parent = cursor;
            leftSibling = i - 1;
            rightSibling = i + 1;
            if (origin < cursor->key[i]) {
                cursor = cursor->ptr[i];
                break;
            }
            if (i == cursor->size - 1) {
                leftSibling = i;
                rightSibling = i + 2;
                cursor = cursor->ptr[i + 1];
                break;
            }
        }
    }
    bool found = false;
    int pos;
    for (pos = 0; pos < cursor->size; pos++) {
        if (cursor->key[pos] == origin) {
            found = true;
            break;
        }
    }
    if (!found) {
        std::cout << "Not found\n";
        return false;
    }
    remove(origin);
    insert();
    return true;
}

template<typename ValueType, typename KeyType>
std::vector<ValueType> BPlusTree<ValueType, KeyType>::queryWithMatch(ValueType value,bool (*compare)(ValueType, ValueType)) {
    std::vector<ValueType>answer;
    if(root== nullptr)return answer;
    Iterator<ValueType,KeyType>iterator= Iterator<ValueType,KeyType>(begin(),end());
    while(iterator.getCur()!= nullptr) {
        Node<KeyType>* node = iterator.getCur();
        Page<ValueType,KeyType>* page=buffer_pool_manager->FetchPage(node->page_id);
        for (int i = 0; i < node->size; i++) {
            auto temp=page->getData(node->key[i]);
            if(compare(value,temp)) {
                answer.push_back(temp);
            }
        }
        buffer_pool_manager->UnpinPage(node->page_id, false);
        iterator.gotoNextNode();
    }
    return answer;
}

template<typename ValueType, typename KeyType>
KeyType BPlusTree<ValueType, KeyType>::getEndKey() {
    auto node =end();
    return node->key[node->size-1];
}

template<typename ValueType, typename KeyType>
Iterator<ValueType, KeyType>::Iterator() {
    m_end= nullptr;
    m_start=nullptr;
    m_cur= nullptr;
    cur=0;
}

template<typename ValueType, typename KeyType>
Iterator<ValueType, KeyType> Iterator<ValueType, KeyType>::operator++(int) {
    Iterator<ValueType,KeyType> iterator= *this;
    if(iterator.m_cur->size==cur+1){
        m_cur=m_cur->ptr[0];
        cur=0;
    }else{
        cur++;
    }
    return iterator;
}

template<typename ValueType, typename KeyType>
Iterator<ValueType, KeyType>::Iterator(Node<KeyType>* begin,Node<KeyType>* end) {
    m_end= end;
    m_start=begin;
    m_cur= begin;
    cur=0;
}

template<typename ValueType, typename KeyType>
Iterator<ValueType, KeyType> &Iterator<ValueType, KeyType>::operator=(const Iterator<ValueType, KeyType> & iterator) {
    m_end= iterator.m_end;
    m_start=iterator.m_start;
    m_cur= iterator.m_cur;
    cur= iterator.cur;
    return *this;
}

template<typename ValueType, typename KeyType>
Node<KeyType> *Iterator<ValueType, KeyType>::getCur() {
    return m_cur;
}

template<typename ValueType, typename KeyType>
void Iterator<ValueType, KeyType>::gotoNextNode() {
    if(m_cur == m_end){
        m_cur= nullptr;
        return;
    }
    m_cur=m_cur->ptr[0];
}




#endif //BOOK_MANAGEMENT_BPLUSTREE_H
