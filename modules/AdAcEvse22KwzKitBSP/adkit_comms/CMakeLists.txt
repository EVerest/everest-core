cmake_minimum_required(VERSION 3.10)

# set the project name
project(adkit_comms VERSION 0.1)
# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_library(adkit_comms STATIC)
ev_register_library_target(adkit_comms)

target_sources(adkit_comms
    PRIVATE
        evSerial.cpp
        protobuf/adkit.pb.c
)

target_include_directories(adkit_comms
    PUBLIC
        "${PROJECT_BINARY_DIR}"
        protobuf
)

target_link_libraries(adkit_comms
    PUBLIC
        date::date-tz
        everest::nanopb
    PRIVATE
        Pal::Sigslot
        everest::framework
        everest::gpio
)
