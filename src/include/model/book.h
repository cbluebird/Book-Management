//
// Created by 蔡瑞凯 on 2023/11/28.
//

#ifndef BOOK_MANAGEMENT_BOOK_H
#define BOOK_MANAGEMENT_BOOK_H

#include <string>

class Book {
    int ID;
    std::string name;
    std::string author;
    std::string type;//类型
    bool state;

    static int book_num;
public:
    std::string getIDString();
    int getId() const;
    void setId(int id);
    const std::string &getName() const;
    void setName(const std::string &name);
    const std::string &getAuthor() const;
    void setAuthor(const std::string &author);
    const std::string &getType() const;
    void setType(const std::string &type);
    bool isState() const;
    void setState(bool state);
    static void setNum(int);
    static int getBookNum();
    std::string to_string();
    Book(){};
    Book(int , std::string , std::string , std::string , bool );
    Book(std::string , std::string , std::string , bool );
    friend std::istream &operator>>(std::istream &in,Book &right);
    friend std::ostream &operator<<(std::ostream &out, const Book &right);
};


#endif //BOOK_MANAGEMENT_BOOK_H
