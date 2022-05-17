set(EVEREST_CROSS_PYTHON_INCLUDE_DIRS "${CMAKE_SYSROOT}/usr/include/python3.9" CACHE STRING "Path to python include directory")
set(EVEREST_CROSS_PYTHON_LIBRARIES "${CMAKE_SYSROOT}/usr/lib/arm-linux-gnueabihf/libpython3.9.so" CACHE STRING "Path to python library")

if (CMAKE_CROSSCOMPILING)
    set(PYTHON_INCLUDE_DIRS ${EVEREST_CROSS_PYTHON_INCLUDE_DIRS} CACHE INTERNAL "")
    set(PYTHON_LIBRARIES ${EVEREST_CROSS_PYTHON_LIBRARIES} CACHE INTERNAL "")
    set(PYTHON_MODULE_EXTENSION ".so" CACHE INTERNAL "")

    set(PYTHONLIBS_FOUND TRUE CACHE INTERNAL "")
endif()

find_program(
    PYTHON_EXECUTABLE
    python3
    REQUIRED
)

CPMAddPackage(
    NAME pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    GIT_TAG v2.9.2
)

CPMAddPackage(
    NAME pybind11_json
    GIT_REPOSITORY https://github.com/pybind/pybind11_json.git
    GIT_TAG 0.2.12
)

pybind11_add_module(everestpy everestpy.cpp)

target_link_libraries(everestpy
    PRIVATE
    everest::framework
    everest::log
    pybind11_json
    fmt::fmt
)

install(
    TARGETS everestpy
    LIBRARY
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/everest/everestpy
)

install(
    FILES
    everest.py
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/everest/everestpy
)
