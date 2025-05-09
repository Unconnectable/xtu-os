cmake_minimum_required(VERSION 3.10)
project(OS_Labs)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 包含头文件路径
include_directories(${PROJECT_SOURCE_DIR}/src/include)

# 添加子模块
add_subdirectory(src)
add_subdirectory(test)
