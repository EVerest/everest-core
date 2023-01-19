find_program(
    PYTHON_EXECUTABLE
    python3
    REQUIRED
)

if (DISABLE_EDM)
    find_package(pybind11 REQUIRED)
    find_package(pybind11_json REQUIRED)
endif()

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
