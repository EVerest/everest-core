cmake_minimum_required(VERSION 3.10)

# set the project name
project(evyeti_comms VERSION 0.1)

configure_file(config.h.in config.h)

# add the executable
add_library(evyeti_comms STATIC)
target_sources(evyeti_comms
    PRIVATE
        evSerial.cpp
        protobuf/hi2lo.pb.c
        protobuf/lo2hi.pb.c
)

target_include_directories(evyeti_comms
    PUBLIC
        "${PROJECT_BINARY_DIR}"
        protobuf
)

target_link_libraries(evyeti_comms
    PUBLIC
        date::date-tz
        everest::nanopb
    PRIVATE
        Pal::Sigslot
        everest::framework
)
