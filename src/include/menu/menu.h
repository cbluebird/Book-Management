//
// Created by 蔡瑞凯 on 2023/11/28.
//

#ifndef BOOK_MANAGEMENT_MENU_H
#define BOOK_MANAGEMENT_MENU_H

#include <iostream>

class Menu {
public:
    void show();
private:
    static void displayAdminMainMenu();
    static void displayDeleteMenu();
    static void displayUpdateMenu();
    static void displayFetchMenu();
    static void displayUserMainMenu();

    static void error();

    static void cutRule();

    void displaySortUser();
};


#endif //BOOK_MANAGEMENT_MENU_H
