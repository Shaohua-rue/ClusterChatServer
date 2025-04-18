cmake_minimum_required(VERSION 3.0)

project(chatRoom)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 配置编译选项 debug
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

# 配置最终的可执行文件输出的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 配置头文件的搜索路径
include_directories(
    ${PROJECT_SOURCE_DIR}/include/
    ${PROJECT_SOURCE_DIR}/include/server
    ${PROJECT_SOURCE_DIR}/include/muduo
    ${PROJECT_SOURCE_DIR}/include/db
    ${PROJECT_SOURCE_DIR}/include/model
    ${PROJECT_SOURCE_DIR}/include/json
    ${PROJECT_SOURCE_DIR}/include/common
    ${PROJECT_SOURCE_DIR}/include/redis
    )

#配置库文件的搜索路径
link_directories(
    ${PROJECT_SOURCE_DIR}/lib
    /usr/lib64/mysql    # mysql库文件路径
    )

# 加载子目录
add_subdirectory(src)