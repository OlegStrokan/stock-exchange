cmake_minimum_required(VERSION 4.1)
project(matching_engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(PUBLIC include)

add_executable(matching_engine src/main.cpp)
