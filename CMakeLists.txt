cmake_minimum_required(VERSION 3.10)

# set the project name
project(IndexBuilderTest VERSION 1.0
                  DESCRIPTION "Testing of IndexBuilder"
                  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(fblib INTERFACE)
include_directories(lib)

# add the executable
add_executable(IndexBuilderTest index/index_test.cpp)

