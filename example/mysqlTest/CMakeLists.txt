cmake_minimum_required(VERSION 3.0)

project(chat)

# 包含头文件目录
include_directories(
    include
)

# 添加可执行文件
add_executable(chat 
    src/main.cpp
)

# 指定库文件搜索路径
#set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /usr/lib64/mysql)
target_link_directories(chat PUBLIC /usr/lib64/mysql)
# 查找 MySQL 客户端库
find_library(MYSQL_CLIENT_LIB mysqlclient)
if(NOT MYSQL_CLIENT_LIB)
    message(FATAL_ERROR "MySQL client library not found.")
endif()

# 链接 MySQL 客户端库
target_link_libraries(chat
    ${MYSQL_CLIENT_LIB}
)