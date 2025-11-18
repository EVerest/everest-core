cmake_minimum_required(VERSION 3.10)

# set the project name
project(mMWcar_mcu_comms VERSION 0.1)
# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_library(mMWcar_mcu_comms STATIC)
target_sources(mMWcar_mcu_comms
    PRIVATE
        evSerial.cpp
        gpio.cpp
        protobuf/mMWcar.pb.c
)

target_include_directories(mMWcar_mcu_comms
    PUBLIC
        "${PROJECT_BINARY_DIR}"
        protobuf
)

target_link_libraries(mMWcar_mcu_comms
    PUBLIC
        date::date-tz
        everest::nanopb
    PRIVATE
        Pal::Sigslot
        everest::framework
)
