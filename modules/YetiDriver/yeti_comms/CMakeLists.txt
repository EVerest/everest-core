cmake_minimum_required(VERSION 3.10)

# set the project name
project(yeti_comms VERSION 0.1)
# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_library(yeti_comms STATIC)
target_sources(yeti_comms
    PRIVATE
        evSerial.cpp
        protobuf/yeti.pb.c
)

target_include_directories(yeti_comms
    PUBLIC
        "${PROJECT_BINARY_DIR}"
        protobuf
)

target_link_libraries(yeti_comms
    PUBLIC
        date::date-tz
        everest::nanopb
    PRIVATE
        Pal::Sigslot
        everest::framework
        everest::gpio
)
