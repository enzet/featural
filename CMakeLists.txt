cmake_minimum_required(VERSION 3.30)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wno-long-long -pedantic")

project(Language)

include_directories(include)
add_subdirectory(src)

add_executable(language src/main.cpp)
