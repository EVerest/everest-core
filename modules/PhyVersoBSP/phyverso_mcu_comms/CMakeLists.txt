cmake_minimum_required(VERSION 3.10)

# set the project name
project(phyverso_mcu_comms VERSION 0.1)
# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_library(phyverso_mcu_comms STATIC)
target_sources(phyverso_mcu_comms
    PRIVATE
        evSerial.cpp
        protobuf/phyverso.pb.c
)

target_include_directories(phyverso_mcu_comms
    PUBLIC
        "${PROJECT_BINARY_DIR}"
        protobuf
)

target_link_libraries(phyverso_mcu_comms
    PUBLIC
        date::date-tz
        everest::nanopb
    PRIVATE
        Pal::Sigslot
        everest::framework
)
