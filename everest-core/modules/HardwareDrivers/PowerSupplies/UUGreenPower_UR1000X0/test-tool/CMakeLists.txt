cmake_minimum_required(VERSION 3.10)

# set the project name
project(uugreen-acdc-test-tool VERSION 0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_executable(uugreen-acdc-test-tool main.cpp ../can_driver_acdc/CanDevice.cpp ../can_driver_acdc/UUGreenCanDevice.cpp ../can_driver_acdc/CanPackets.cpp)
target_include_directories(uugreen-acdc-test-tool PRIVATE ".." "../can_driver_acdc" ${everest-framework_SOURCE_DIR})
target_link_libraries(uugreen-acdc-test-tool PRIVATE Pal::Sigslot date::date date::date-tz everest::log)
