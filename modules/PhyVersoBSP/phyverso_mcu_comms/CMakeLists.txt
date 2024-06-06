cmake_minimum_required(VERSION 3.10)

# set the project name
project(phyverso_mcu_comms VERSION 0.1)
# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(phyverso_config STATIC)
target_sources(phyverso_config
    PRIVATE
        evConfig.cpp
)

target_include_directories(phyverso_config
    PUBLIC
        "${PROJECT_BINARY_DIR}"
        protobuf
)

target_link_libraries(phyverso_config
    PRIVATE
        everest::nanopb
        nlohmann_json::nlohmann_json
)

add_library(phyverso_mcu_comms STATIC)
target_sources(phyverso_mcu_comms
    PRIVATE
        evSerial.cpp
        protobuf/phyverso.pb.c
        bsl/bsl_gpio.cpp
)

target_include_directories(phyverso_mcu_comms
    PUBLIC
        "${PROJECT_BINARY_DIR}"
        protobuf
        bsl
)

target_link_libraries(phyverso_mcu_comms
    PUBLIC
        date::date-tz
        everest::nanopb
    PRIVATE
        Pal::Sigslot
        everest::framework
        phyverso_config
)
