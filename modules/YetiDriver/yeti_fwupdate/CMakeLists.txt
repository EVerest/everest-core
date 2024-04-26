cmake_minimum_required(VERSION 3.10)

# set the project name
project(yeti_fwupdate VERSION 0.1)

# add the executable
add_executable(yeti_fwupdate main.cpp)
target_include_directories(yeti_fwupdate PUBLIC "${PROJECT_BINARY_DIR}" PUBLIC "../yeti_comms/nanopb" PUBLIC "../yeti_comms/protobuf" PUBLIC "../yeti_comms")
target_link_libraries(yeti_fwupdate PRIVATE Pal::Sigslot Threads::Threads yeti_comms everest::framework)

install(TARGETS yeti_fwupdate)


