//
// Created by 蔡瑞凯 on 2023/11/28.
//

#include "../include/model/book.h"
#include <iostream>

int Book::book_num=0;

int Book::getId() const {
    return ID;
}

void Book::setId(int id) {
    ID = id;
}

const std::string &Book::getName() const {
    return name;
}

void Book::setName(const std::string &name) {
    Book::name = name;
}

const std::string &Book::getAuthor() const {
    return author;
}

void Book::setAuthor(const std::string &author) {
    Book::author = author;
}

const std::string &Book::getType() const {
    return type;
}

void Book::setType(const std::string &type) {
    Book::type = type;
}

bool Book::isState() const {
    return state;
}

void Book::setState(bool state) {
    Book::state = state;
}

int Book::getBookNum() {
    return book_num;
}

std::string Book::getIDString(){
    std::string s;
    int x=ID;
    while(x>0){
        s=std::to_string(x%10)+s;
        x/=10;
    }
    return s;
}

std::string Book::to_string(){
    std::string result;
    auto BookID=getIDString();
    std::string state_string=state?"借出":"在库";
    result=BookID+"  "+name+"  "+author+"  "+type+" "+state_string;
    return result;
}

Book::Book(int ID, std::string name, std::string author, std::string type, bool state):ID(ID),name(name),author(author),type(type),state(state){}

Book::Book(std::string name, std::string author, std::string type, bool state):name(name),author(author),type(type),state(state) {
    this->ID=++Book::book_num;
}

void Book::setNum(int num_) {
       book_num=num_;
}

std::istream &operator>>(std::istream &in, Book &right) {
    if(typeid(in) != typeid(std::cin)) {
        in >> right.ID;
    }

    if(typeid(in) == typeid(std::cin)) {
        std::cout << "请输入书名的姓名（输入0代表为留空）：";
    }
    in >> right.name;
    if(right.name=="0") {
        right.name = "";
    }

    if(typeid(in) == typeid(std::cin)) {
        std::cout <<"请输入" << right.name << "的作者姓名（输入0代表为留空）：";
    }

    in >> right.author;
    if(right.author=="0") {
        right.author = "";
    }

    if(typeid(in) == typeid(std::cin)) {
        std::cout <<"请输入" << right.name << "的类别（输入0代表为留空）：";
    }
    in >> right.type;
    if(right.type=="0") {
        right.type = "";
    }

    std::string state;
    if(typeid(in) == typeid(std::cin)) {
        std::cout <<"请输入" << right.name << "的状态（输入0代表为在库中，1代表出借）：";
    }
    in>>state;
    if(state=="0")right.state= false;
    else right.state=true;

    return in;
}

std::ostream &operator<<(std::ostream &out, const Book &right) {
    if(typeid(out) == typeid(std::cout)) {
        out <<"书本id:"<<right.ID<<'\n'
            << "书名:" << right.name << '\n'
            << "作者:" << right.author<< "\n"
            << "类别:" << right.type<< '\n'
            << "状态:"<<(right.state?"借出":"在库中")<< std::endl;
    } else {
        out<<right.ID<<" ";
        if(right.name.empty()) {
            out << "0" << " ";
        } else {
            out << right.name << " ";
        }
        if(right.author.empty()) {
            out << "0" << " ";
        } else {
            out << right.author << " ";
        }
        if(right.type.empty()) {
            out << "0" << " ";
        } else {
            out << right.type<< " ";
        }
        if(right.state) {
            out << "1" << " ";
        } else {
            out << "0" << " ";
        }
    }
    return out;
}


