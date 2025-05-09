# src/lab3/CMakeLists.txt

# 创建静态库 libMemoryAlloc.a
if(EXISTS ${PROJECT_SOURCE_DIR}/src/lab3/MemoryAlloc.cpp)
    add_library(MemoryAlloc STATIC MemoryAlloc.cpp)
else()
    add_library(MemoryAlloc STATIC MemoryAlloc.c)
endif()