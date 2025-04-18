cmake_minimum_required(VERSION 3.0)

project(muduo_test)

include_directories(
    ../../include/muduo
)

add_executable(
    muduo_test
    main.cpp
)

# 修正目标名称为 muduo_test
target_link_directories(muduo_test PUBLIC
    ../../lib
)

target_link_libraries(
    muduo_test
    muduo_core
    pthread
)