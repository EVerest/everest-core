cmake_minimum_required(VERSION 3.10)

# set the project name
project(ad-acevse22kwz-kit_fwupdate VERSION 0.1)
# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_executable(ad-acevse22kwz-kit_fwupdate main.cpp)
target_include_directories(ad-acevse22kwz-kit_fwupdate PUBLIC "${PROJECT_BINARY_DIR}" PUBLIC "../ad-acevse22kwz-kit_comms/nanopb" PUBLIC "../ad-acevse22kwz-kit_comms/protobuf" PUBLIC "../ad-acevse22kwz-kit_comms")
target_link_libraries(ad-acevse22kwz-kit_fwupdate PRIVATE Pal::Sigslot Threads::Threads ad-acevse22kwz-kit_comms everest::framework)

install(TARGETS ad-acevse22kwz-kit_fwupdate)


