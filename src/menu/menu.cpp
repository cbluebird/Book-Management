//
// Created by 蔡瑞凯 on 2023/11/28.
//

#include "../include/menu/menu.h"


void Menu::displayAdminMainMenu() {
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "请输入对应功能前的编号来操作图书管理系统" << std::endl
              << "1: 添加图书信息" << std::endl
              << "2: 查询图书信息" << std::endl
              << "3: 修改图书信息" << std::endl
              << "4: 删除图书信息" << std::endl
              << "0: 退出系统并保存数据" << std::endl;
    std::cout << ">";
}

void Menu::displayFetchMenu() {
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "请输入对应功能前的编号来查询信息" << std::endl
              << "1: 根据用户姓名进行精确查询" << std::endl
              << "2: 根据作者进行精确查询" << std::endl
              << "3: 根据书名进行模糊查询" << std::endl
              << "4：查询所有书本信息"<<std::endl
              << "5: 根据书本ID进行精确查询"<<std::endl
              << "6: 根据类别查询" << std::endl
              <<"7: 根据书名查询库存" << std::endl
              << "0: 返回上级菜单" << std::endl;
    std::cout << ">";
}

void Menu::displayUpdateMenu() {
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "请输入对应功能前的编号来修改信息" << std::endl
              << "1: 根据书籍id进行修改信息" << std::endl
              << "2: 根据书籍id更新书本状态" << std::endl
              << "0: 返回上级菜单" << std::endl;
    std::cout << ">";
}

void Menu::displayDeleteMenu() {
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "请输入对应功能前的编号来删除用户" << std::endl
              << "1: 根据书本id进行删除" << std::endl
              << "0: 返回上级菜单" << std::endl;
    std::cout << ">";
}

void Menu::displayUserMainMenu() {
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "请输入对应功能前的编号来操作通讯录管理系统" << std::endl
              << "1: 查询图书信息" << std::endl
              << "0: 退出系统并保存数据" << std::endl;
    std::cout << ">";
}

void Menu::error(){
    std::cout << "输入有误，请重新输入\n>";
}

void Menu::cutRule() {
    std::cout << "----------------------------------------" << std::endl;
}

