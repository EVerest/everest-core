cmake_minimum_required(VERSION 3.10)

# set the project name
project(pn532_serial VERSION 0.1)
# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)


set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# add the executable
add_library(pn532_serial STATIC PN532Serial.cpp)
ev_register_library_target(pn532_serial)

target_include_directories(pn532_serial PUBLIC "${PROJECT_BINARY_DIR}")
target_link_libraries(pn532_serial PRIVATE Threads::Threads everest::framework)
