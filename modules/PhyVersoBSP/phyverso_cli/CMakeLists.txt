cmake_minimum_required(VERSION 3.10)

# set the project name
project(phyverso_cli VERSION 0.1)
# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_executable(phyverso_cli main.cpp)
target_include_directories(phyverso_cli PUBLIC "${PROJECT_BINARY_DIR}" PUBLIC "../phyverso_mcu_comms/protobuf" PUBLIC "../phyverso_mcu_comms")
target_link_libraries(phyverso_cli PRIVATE Pal::Sigslot Threads::Threads phyverso_mcu_comms everest::framework everest::nanopb)

install(TARGETS phyverso_cli
        DESTINATION ${EVEREST_MOD_YETIDRIVER_DESTINATION})


