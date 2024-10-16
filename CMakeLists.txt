cmake_minimum_required(VERSION 3.14)

project(everest-utils
    VERSION 0.3.0
    DESCRIPTION "A collection of utilities for the EVerest project"
    LANGUAGES CXX C
)

find_package(everest-cmake 0.3 REQUIRED
    PATHS ../everest-cmake
)

ev_setup_cmake_variables_python_wheel()

ev_add_pip_package(
    NAME ev-dev-tools
    SOURCE_DIRECTORY ev-dev-tools
)

add_subdirectory(everest-testing)
