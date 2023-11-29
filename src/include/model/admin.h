//
// Created by 蔡瑞凯 on 2023/11/29.
//

#ifndef BOOK_MANAGEMENT_ADMIN_H
#define BOOK_MANAGEMENT_ADMIN_H


#include <string>
#include <fstream>
#include <iostream>

class Admin {
public:
    Admin(std::string, std::string);

    bool login();

private:
    std::string name;
    std::string pwd;
    std::string path;
    std::string type;

};


#endif //BOOK_MANAGEMENT_ADMIN_H
