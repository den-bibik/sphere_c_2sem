cmake_minimum_required(VERSION 3.1)
project(01_allocator)

#set(CMAKE_CXX_STANDARD 11)

set(GTEST_LIB_SRC
        ../thirdparty/gtest/gtest-all.cc
        ../thirdparty/gtest/gtest_main.cc
        )


set(CXX_FLAGS "-g -std=c++11 -pthread")

set(SOURCE_FILES
        allocator.cpp allocator_error.cpp allocator_pointer.cpp allocator_test.cpp
         )
set(HEADERS
        allocator.h allocator_error.h allocator_pointer.h
        )

include_directories(../thirdparty/)

add_library(gtest ${GTEST_LIB_SRC})
#set_target_properties(gtest PROPERTIES COMPILE_FLAGS ${CXX_FLAGS})

add_executable(01_allocator ${SOURCE_FILES} ${HEADERS})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(01_allocator Threads::Threads)

target_link_libraries(01_allocator gtest)
set_target_properties(01_allocator PROPERTIES COMPILE_FLAGS ${CXX_FLAGS})


