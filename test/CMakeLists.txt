enable_testing()

# 包含头文件路径
include_directories(${PROJECT_SOURCE_DIR}/src/include/lab2)

# 添加测试程序，并指定正确的源文件路径
add_executable(banker_test lab2/banker_test.cpp)

# 链接 Google Test
target_link_libraries(banker_test PRIVATE gtest gtest_main pthread banker)

# 注册到 CTest
add_test(NAME BankerSafeSeqTest COMMAND banker_test)