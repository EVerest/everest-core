cmake_minimum_required(VERSION 3.10)

# set the project name
project(phyverso_gpio VERSION 0.1)
# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(phyverso_gpio STATIC)
target_sources(phyverso_gpio
    PRIVATE
        evGpio.cpp
)

target_include_directories(phyverso_gpio
    PUBLIC
        "${PROJECT_BINARY_DIR}"
        "../phyverso_mcu_comms"
)

target_link_libraries(phyverso_gpio
    PUBLIC
        date::date-tz
        everest::nanopb
    PRIVATE
        Pal::Sigslot
        everest::framework
        everest::gpio
        phyverso_config
        fmt::fmt
)
