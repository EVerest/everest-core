cmake_minimum_required(VERSION 3.10)

# set the project name
project(ad-acevse22kwz-kit_comms VERSION 0.1)
# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_library(ad-acevse22kwz-kit_comms STATIC)
ev_register_library_target(ad-acevse22kwz-kit_comms)

target_sources(ad-acevse22kwz-kit_comms
    PRIVATE
        evSerial.cpp
        protobuf/ad-acevse22kwz-kit.pb.c
)

target_include_directories(ad-acevse22kwz-kit_comms
    PUBLIC
        "${PROJECT_BINARY_DIR}"
        protobuf
)

target_link_libraries(ad-acevse22kwz-kit_comms
    PUBLIC
        date::date-tz
        everest::nanopb
    PRIVATE
        Pal::Sigslot
        everest::framework
        everest::gpio
)
