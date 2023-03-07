cmake_minimum_required(VERSION 3.24)
set(CMAKE_C_STANDARD 11)

set(SRC_FILES example.c)

add_executable(example ${SRC_FILES})
target_include_directories(example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(example leakfree)