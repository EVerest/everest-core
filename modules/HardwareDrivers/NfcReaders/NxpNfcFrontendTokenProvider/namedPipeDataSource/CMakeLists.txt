# This is a standalone file, to compile the namedPipeTokenProvider as standalone executable

cmake_minimum_required(VERSION 3.15)

project(testNamedPipeDataSource)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(namedPipeDataSource)
target_sources(namedPipeDataSource PRIVATE namedPipeDataSource.cpp)
target_include_directories(namedPipeDataSource PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
