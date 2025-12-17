cmake_minimum_required(VERSION 3.14)

project(everest-utils
    VERSION 0.7.3
    DESCRIPTION "A collection of utilities for the EVerest project"
    LANGUAGES CXX C
)

find_package(everest-cmake 0.5 REQUIRED
    PATHS ../everest-cmake
)

ev_setup_cmake_variables_python_wheel()

ev_add_pip_package(
    NAME ev-dev-tools
    SOURCE_DIRECTORY ev-dev-tools
)

ev_add_pip_package(
    NAME everest-testing
    SOURCE_DIRECTORY everest-testing
)

add_subdirectory(everest-testing)
