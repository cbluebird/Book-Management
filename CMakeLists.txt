cmake_minimum_required(VERSION 3.26)
project(Book_Management)

set(CMAKE_CXX_STANDARD 20)

add_executable(Book_Management src/main.cpp
        src/storage/page/page.cpp
        src/storage/filter/filter.cpp
        src/model/book.cpp
        src/storage/disk/diskManager.cpp
        src/storage/disk/disk.cpp
        src/menu/menu.cpp
        src/model/admin.cpp
)
