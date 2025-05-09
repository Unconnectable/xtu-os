add_library(banker STATIC banker.cpp)

target_include_directories(banker PUBLIC ${PROJECT_SOURCE_DIR}/src/include/lab2)