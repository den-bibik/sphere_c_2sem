cmake_minimum_required(VERSION 3.6)
project(02_skiplist)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        include/skiplist/iterator.h
        include/skiplist/node.h
        include/skiplist/skiplist.h
        test/skiplist_test.cpp)

add_executable(02_skiplist ${SOURCE_FILES})