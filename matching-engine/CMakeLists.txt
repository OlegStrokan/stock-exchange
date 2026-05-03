cmake_minimum_required(VERSION 3.29)
project(matching_engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(PUBLIC include)

add_executable(matching_engine
    src/main.cpp
    src/MatchingEngine.cpp
    src/OrderBook.cpp
)

add_executable(benchmark
    src/benchmark.cpp
    src/MatchingEngine.cpp
    src/OrderBook.cpp
)
