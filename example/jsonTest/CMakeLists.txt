cmake_minimum_required(VERSION 3.0)

project(json_test)

include_directories(
    ../../include
)
add_executable(
    json_test
    testjson.cpp
)