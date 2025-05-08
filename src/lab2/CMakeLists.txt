#add_executable(banker banker.cpp)

#target_include_directories(banker PRIVATE ${PROJECT_SOURCE_DIR}/src/include/lab2)

add_library(banker STATIC banker.cpp)

target_include_directories(banker PUBLIC ${PROJECT_SOURCE_DIR}/src/include/lab2)