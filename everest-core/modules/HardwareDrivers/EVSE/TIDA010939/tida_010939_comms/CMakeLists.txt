cmake_minimum_required(VERSION 3.10)

# set the project name
project(tida_010939_comms VERSION 0.1)
# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_library(tida_010939_comms STATIC)
ev_register_library_target(tida_010939_comms)

target_sources(tida_010939_comms
    PRIVATE
        evSerial.cpp
        protobuf/tida010939.pb.c
)

target_include_directories(tida_010939_comms
    PUBLIC
        "${PROJECT_BINARY_DIR}"
        protobuf
)

target_link_libraries(tida_010939_comms
    PUBLIC
        date::date-tz
        everest::nanopb
    PRIVATE
        Pal::Sigslot
        everest::framework
        everest::gpio
)
