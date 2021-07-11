cmake_minimum_required(VERSION 3.0)
project(yaa)

set(CMAKE_CXX_STANDARD 20)

# add_definitions(-DDEBUG)

include_directories(src)

add_executable(yaa src/main.cpp src/allocator.h src/page.cpp src/page.h src/test.h)