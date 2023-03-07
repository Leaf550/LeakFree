cmake_minimum_required(VERSION 3.24)

set(CMAKE_C_STANDARD 11)

set(SRC_FILES leakfree.c)

add_library(leakfree ${SRC_FILES})
target_include_directories(leakfree PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(leakfree PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})