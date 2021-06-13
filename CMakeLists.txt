cmake_minimum_required(VERSION 3.0)
project(yaa)

set(CMAKE_CXX_STANDARD 20)

include_directories(src)

add_executable(yaa src/main.cpp src/naive_allocator.h src/allocator.h src/page.cpp src/page.h src/test.h)