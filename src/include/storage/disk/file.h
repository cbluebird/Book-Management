//
// Created by 蔡瑞凯 on 2023/11/28.
//

#ifndef BOOK_MANAGEMENT_FILE_H
#define BOOK_MANAGEMENT_FILE_H



#include <fstream>
#include<string>
#include<vector>
#include<unordered_map>

template<typename ValueType,typename KeyType>
class File {
    std::string path;
    std::string fileType;
public:
    File(const std::string& path, const std::string& fileType) : path(path), fileType(fileType){
        std::ofstream out;
        out.open(path + fileType, std::ios::out | std::ios::app);
        out.close();
    };
    std::unordered_map<KeyType,ValueType> init(KeyType(*getMajor)(ValueType));
    void update(std::vector<ValueType>&);
    void insert(ValueType);
    std::unordered_map<KeyType,ValueType> getData();
};


#endif //BOOK_MANAGEMENT_FILE_H
