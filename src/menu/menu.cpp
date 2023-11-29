//
// Created by 蔡瑞凯 on 2023/11/28.
//

#include "../include/menu/menu.h"
#include "../include/model/book.h"
#include "../include/storage/table/table.h"
#include "../include/model/admin.h"

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

void Menu::show() {
    Table<Book,int> bookDB=Table<Book,int>("../../data/data", ".txt",[](Book book){return book.getId();});
    Book::setNum(bookDB.getEndKey());
    int userType = 1;
    bool flagMain = true;
    bool flagMain_ = true;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "欢迎使用图书管理系统" << std::endl;
    std::cout << "请选择登录身份" << std::endl;
    std::cout << "1: 管理员" << std::endl;
    std::cout << "2: 用户" << std::endl;
    std::cout << "0: 退出" << std::endl;
    std::cout << ">";
    while(flagMain && std::cin >> userType && userType) {
        int typeNum;
        if(userType ==1){
            Admin admin("../../data/admin", ".txt");
            if(!admin.login()) {
                flagMain_ = false;
                break;
            }
            this->displayAdminMainMenu();
            while (flagMain_ && std::cin >> typeNum) {
                switch (typeNum) {
                    case 1: {
                        Book book;
                        std::cin>>book;
                        int x=book.getBookNum();
                        book.setId(++x);
                        book.setNum(x);
                        bookDB.insert(x,book);
                        break;
                    }
                    case 2: {
                        int flag=1;
                        int typeNum_=0;
                        this->displayFetchMenu();
                        while(flag&&std::cin>>typeNum_){
                            switch (typeNum_) {
                                case 1: {
                                    this->cutRule();
                                    std::cout<<"请输入需要查询的书名: ";
                                    Book book;
                                    std::string name;
                                    std::cin>>name;
                                    book.setName(name);
                                    auto ans=bookDB.queryWithMatch(book, [](Book book1, Book book2){
                                        if(book1.getName() == book2.getName())return true;
                                        else return false;
                                    });
                                    if(ans.size()==0){
                                        std::cout<<"输入的书名不存在"<<std::endl;
                                        break;
                                    }
                                    for(int i=0;i<ans.size();i++){
                                        std::cout<<ans[i]<<std::endl;
                                    }
                                    break;
                                }
                                case 2: {
                                    this->cutRule();
                                    std::cout<<"请输入需要查询的作者: ";
                                    Book book;
                                    std::string author;
                                    std::cin >> author;
                                    book.setAuthor(author);
                                    auto ans=bookDB.queryWithMatch(book, [](Book user1, Book user2){
                                        if(user1.getAuthor()==user2.getAuthor())return true;
                                        else return false;
                                    });
                                    if(ans.size()==0){
                                        std::cout<<"输入的作者不存在"<<std::endl;
                                        break;
                                    }
                                    for(int i=0;i<ans.size();i++){
                                        std::cout<<ans[i]<<std::endl;
                                    }
                                    break;
                                }
                                case 3: {
                                    this->cutRule();
                                    std::cout<<"请输入需要模糊查询的书名: ";
                                    Book book;
                                    std::string name;
                                    std::cin >> name;
                                    book.setName(name);
                                    auto ans=bookDB.queryWithMatch(book, [](Book user1, Book user2){
                                        int f=0,flag_=0;
                                        std::string name=user2.getName();
                                        std::string mh=user1.getName();
                                        for(int i=0;i<name.size()&&!flag_;i++){
                                            if(name[i]==mh[f]){
                                                f++;
                                            }
                                            if(f==mh.size()-1)flag_=1;
                                        }
                                        if(flag_)return true;
                                        else return false;
                                    });
                                    if(ans.empty()){
                                        std::cout<<"书本不存在"<<std::endl;
                                        break;
                                    }
                                    for(const auto & an : ans){
                                        std::cout<<an<<std::endl;
                                    }
                                    break;
                                }
                                case 4: {
                                    this->cutRule();
                                    auto ans=bookDB.queryAll();
                                    for(const auto & an : ans){
                                        std::cout<<an<<std::endl;
                                    }
                                    break;
                                }
                                case 5: {
                                    this->cutRule();
                                    std::cout<<"请输入需要查询的书本id: ";
                                    int bookID;
                                    std::cin >> bookID;
                                    auto ans=bookDB.querySingleWithKey(bookID);
                                    if(ans== nullptr){
                                        std::cout<<"书本不存在"<<std::endl;
                                        break;
                                    }
                                    std::cout<<*ans<<std::endl;
                                    break;
                                }
                                case 6: {
                                    this->cutRule();
                                    std::cout<<"请输入查询的类别: ";
                                    Book book;
                                    std::string l;
                                    std::cin>>l;
                                    book.setType(l);
                                    auto ans=bookDB.queryWithMatch(book, [](Book user1, Book user2){
                                        if(user1.getType()==user2.getType())return true;
                                        else return false;
                                    });
                                    if(ans.size()==0){
                                        std::cout<<"输入的类别不存在"<<std::endl;
                                        break;
                                    }
                                    for(int i=0;i<ans.size();i++){
                                        std::cout<<ans[i]<<std::endl;
                                    }
                                    break;
                                }
                                case 0:{
                                    flag=0;
                                    break;
                                }
                                default: {
                                    std::cout << "输入错误" << std::endl;
                                    break;
                                }
                            }
                            if(flag)this->displayFetchMenu();
                        }
                        break;
                    }
                    case 3: {
                        int typeNum_=0;
                        this->displayUpdateMenu();
                        std::cin>>typeNum_;
                        if(typeNum_==1){
                            std::cout<<"请选择你要更新的图书信息的ID:";
                            int book_id;
                            std::cin>>book_id;
                            auto book=bookDB.querySingleWithKey(book_id);
                            if(book == nullptr){
                                std::cout<<"该书本不存在"<<std::endl;
                                break;
                            }
                            std::cout<<*book;
                            this->cutRule();
                            Book update_book;
                            update_book.setId(book_id);
                            std::cout<<"请输入你要更新的书本信息"<<std::endl;
                            std::cin >> update_book;
                            if(update_book.getName().empty())update_book.setName(book->getName());
                            if(update_book.getAuthor().empty())update_book.setAuthor(book->getAuthor());
                            if(update_book.getType().empty())update_book.setType(book->getType());
                            bookDB.updateWithoutKey(update_book);
                        } else if(typeNum_==2){
                            std::cout<<"请选择你要更新的图书信息的ID:";
                            int book_id;
                            std::cin>>book_id;
                            auto book=bookDB.querySingleWithKey(book_id);
                            if(book == nullptr){
                                std::cout<<"该书本不存在"<<std::endl;
                                break;
                            }
                            std::cout<<*book;
                            this->cutRule();
                            std::cout<<"请输入你要更新的书本状态"<<std::endl;
                            bool state_;
                            std::cin >> state_;
                            book->setState(state_);
                            bookDB.updateWithoutKey(*book);
                        }else if(typeNum_!=0){
                            this->error();
                        }
                        break;
                    }
                    case 4: {
                        int typeNum_=0;
                        this->displayDeleteMenu();
                        std::cin>>typeNum_;
                        if(typeNum_==1){
                            std::cout<<"请选择你要删除的书本信息的编号:"<<std::endl;
                            int user_id;
                            std::cin>>user_id;
                            auto user=bookDB.querySingleWithKey(user_id);
                            if(user== nullptr){
                                std::cout<<"该书不存在"<<std::endl;
                                break;
                            }
                            std::cout<<*user;
                            this->cutRule();
                            std::cout<<"确定要删除该书本信息吗，确定请输入1,取消选择2"<<std::endl<<">";
                            int flag;
                            std::cin>>flag;
                            if(flag==1){
                                bookDB.remove(user_id);
                            }
                            break;
                        }else if(typeNum_==3){
                            this->cutRule();
                            std::cout<<"请输入需要删除的书本名：";
                            std::string name;
                            std::cin >> name;
                            Book book;
                            book.setName(name);
                            auto ans=bookDB.queryWithMatch(book,[](Book user1,Book user2){
                                if(user1.getName()==user2.getName())return true;
                                else return false;
                            });
                            if(ans.size()==0){
                                std::cout<<"输入电话不存在"<<std::endl;
                                break;
                            }
                            for(int i=0;i<ans.size();i++){
                                std::cout<<ans[i]<<std::endl;
                            }
                            if(ans.size()==0){
                                this->error();
                                break;
                            }
                            std::cout<<"请选择你要删除的用户信息的编号:"<<std::endl;
                            int user_id;
                            std::cin>>user_id;
                            int flag=1;
                            for(int i=0;i<ans.size()&&flag;i++){
                                if(ans[i].getId()==user_id){
                                    flag=0;
                                }
                            }
                            if(flag){
                                this->error();
                                break;
                            }
                            this->cutRule();
                            std::cout<<"确定要删除该用户信息吗，确定请输入1,取消选择2"<<std::endl<<">";
                            std::cin>>flag;
                            if(flag==1){
                                bookDB.remove(user_id);
                            }
                            break;
                        }else{
                            this->error();
                        }
                        break;
                    }
                    case 0: {
                        flagMain=false;
                        flagMain_ = false;
                        break;
                    }
                    default: {
                        error();
                        break;
                    }
                }
                if(flagMain)this->displayAdminMainMenu();
            }
        }else if(userType ==2){
            this->displayUserMainMenu();
            while (flagMain_ && std::cin >> typeNum) {
                switch (typeNum) {
                    case 1: {
                        int flag=1;
                        int typeNum_=0;
                        displayFetchMenu();
                        while(flag&&std::cin>>typeNum_){
                            switch (typeNum_) {
                                int flag=1;
                                int typeNum_=0;
                                this->displayFetchMenu();
                                while(flag&&std::cin>>typeNum_){
                                    switch (typeNum_) {
                                        case 1: {
                                            this->cutRule();
                                            std::cout<<"请输入需要查询的书名: ";
                                            Book book;
                                            std::string name;
                                            std::cin>>name;
                                            book.setName(name);
                                            auto ans=bookDB.queryWithMatch(book, [](Book book1, Book book2){
                                                if(book1.getName() == book2.getName())return true;
                                                else return false;
                                            });
                                            if(ans.size()==0){
                                                std::cout<<"输入的书名不存在"<<std::endl;
                                                break;
                                            }
                                            for(int i=0;i<ans.size();i++){
                                                std::cout<<ans[i]<<std::endl;
                                            }
                                            break;
                                        }
                                        case 2: {
                                            this->cutRule();
                                            std::cout<<"请输入需要查询的作者: ";
                                            Book book;
                                            std::string author;
                                            std::cin >> author;
                                            book.setAuthor(author);
                                            auto ans=bookDB.queryWithMatch(book, [](Book user1, Book user2){
                                                if(user1.getAuthor()==user2.getAuthor())return true;
                                                else return false;
                                            });
                                            if(ans.size()==0){
                                                std::cout<<"输入的作者不存在"<<std::endl;
                                                break;
                                            }
                                            for(int i=0;i<ans.size();i++){
                                                std::cout<<ans[i]<<std::endl;
                                            }
                                            break;
                                        }
                                        case 3: {
                                            this->cutRule();
                                            std::cout<<"请输入需要模糊查询的书名: ";
                                            Book book;
                                            std::string name;
                                            std::cin >> name;
                                            book.setName(name);
                                            auto ans=bookDB.queryWithMatch(book, [](Book user1, Book user2){
                                                int f=0,flag_=0;
                                                std::string name=user2.getName();
                                                std::string mh=user1.getName();
                                                for(int i=0;i<name.size()&&!flag_;i++){
                                                    if(name[i]==mh[f]){
                                                        f++;
                                                    }
                                                    if(f==mh.size()-1)flag_=1;
                                                }
                                                if(flag_)return true;
                                                else return false;
                                            });
                                            if(ans.empty()){
                                                std::cout<<"书本不存在"<<std::endl;
                                                break;
                                            }
                                            for(const auto & an : ans){
                                                std::cout<<an<<std::endl;
                                            }
                                            break;
                                        }
                                        case 4: {
                                            this->cutRule();
                                            auto ans=bookDB.queryAll();
                                            for(const auto & an : ans){
                                                std::cout<<an<<std::endl;
                                            }
                                            break;
                                        }
                                        case 5: {
                                            this->cutRule();
                                            std::cout<<"请输入需要查询的书本id: ";
                                            int bookID;
                                            std::cin >> bookID;
                                            auto ans=bookDB.querySingleWithKey(bookID);
                                            if(ans== nullptr){
                                                std::cout<<"书本不存在"<<std::endl;
                                                break;
                                            }
                                            std::cout<<*ans<<std::endl;
                                            break;
                                        }
                                        case 6: {
                                            this->cutRule();
                                            std::cout<<"请输入查询的类别: ";
                                            Book book;
                                            std::string l;
                                            std::cin>>l;
                                            book.setType(l);
                                            auto ans=bookDB.queryWithMatch(book, [](Book user1, Book user2){
                                                if(user1.getType()==user2.getType())return true;
                                                else return false;
                                            });
                                            if(ans.size()==0){
                                                std::cout<<"输入的类别不存在"<<std::endl;
                                                break;
                                            }
                                            for(int i=0;i<ans.size();i++){
                                                std::cout<<ans[i]<<std::endl;
                                            }
                                            break;
                                        }
                                        case 0:{
                                            flag=0;
                                            break;
                                        }
                                        default: {
                                            std::cout << "输入错误" << std::endl;
                                            break;
                                        }
                                    }
                                    if(flag)this->displayFetchMenu();
                                }
                                break;
                            }
                            if(flag)this->displayFetchMenu();
                        }
                        break;
                    }
                    case 0: {
                        flagMain=false;
                        flagMain_ = false;
                        break;
                    }
                    default: {
                        this->error();
                        break;
                    }
                }
                if(flagMain)this->displayUserMainMenu();
            }
        }else if(userType ==0){
            flagMain= false;
        }else{
            this->error();
        }
    }
}
