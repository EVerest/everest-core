cmake_minimum_required(VERSION 3.10)

# set the project name
project(phyverso_cli VERSION 0.1)
# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Threads REQUIRED)

# add the executable
add_executable(phyverso_cli main.cpp)
target_include_directories(phyverso_cli
    PUBLIC
    "${PROJECT_BINARY_DIR}"
    "../phyverso_mcu_comms/protobuf"
    "../phyverso_mcu_comms"
    "../phyverso_config"
)
target_link_libraries(phyverso_cli
    PRIVATE
        Pal::Sigslot
        Threads::Threads
        phyverso_mcu_comms
        everest::framework
        everest::nanopb
        phyverso_config
)

install(TARGETS phyverso_cli
        DESTINATION ${EVEREST_MOD_PHYVERSOBSP_DESTINATION})
