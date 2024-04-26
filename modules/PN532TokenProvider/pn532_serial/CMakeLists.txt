cmake_minimum_required(VERSION 3.10)

# set the project name
project(pn532_serial VERSION 0.1)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# add the executable
add_library(pn532_serial STATIC PN532Serial.cpp)

target_include_directories(pn532_serial PUBLIC "${PROJECT_BINARY_DIR}")
target_link_libraries(pn532_serial PRIVATE Threads::Threads everest::framework)
