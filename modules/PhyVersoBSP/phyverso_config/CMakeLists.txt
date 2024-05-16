cmake_minimum_required(VERSION 3.10)

# set the project name
project(phyverso_config VERSION 0.1)
# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#find_package(nlohmann_json 3.2.0 REQUIRED)

# add the executable
add_library(phyverso_config STATIC)
target_sources(phyverso_config
    PRIVATE
        evConfig.cpp
)

target_include_directories(phyverso_config
    PUBLIC
        "${PROJECT_BINARY_DIR}"
        "../phyverso_mcu_comms/protobuf"
)

target_link_libraries(phyverso_config
    PRIVATE
        everest::nanopb
        nlohmann_json::nlohmann_json
)
