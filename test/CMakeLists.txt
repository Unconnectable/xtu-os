enable_testing()

# 包含头文件路径
include_directories(${PROJECT_SOURCE_DIR}/src/include)

# 添加测试程序,并指定正确的源文件路径`
add_executable(banker_test lab2/banker_test.cpp)
add_executable(MemoryAlloc_test lab3/MemoryAlloc_test.cpp)
add_executable(PageReplacer_test lab4/PageReplacer_test.cpp)

# 链接 Google Test
target_link_libraries(banker_test PRIVATE gtest gtest_main pthread banker)
target_link_libraries(MemoryAlloc_test PRIVATE gtest gtest_main pthread MemoryAlloc)
target_link_libraries(PageReplacer_test PRIVATE gtest gtest_main pthread PageReplacer)


# 注册到 CTest
add_test(NAME BankerSafeSeqTest COMMAND banker_test)
add_test(NAME MemoryAllocTest COMMAND MemoryAlloc_test)
add_test(NAME PageReplacerTest COMMAND PageReplacer_test)