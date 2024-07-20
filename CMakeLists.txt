cmake_minimum_required(VERSION 3.30)

set (CMAKE_CXX_STANDARD 20)

project(Language)

include_directories(include)
add_subdirectory(src)

add_executable(language src/main.cpp)
target_link_libraries(language /opt/homebrew/lib)
