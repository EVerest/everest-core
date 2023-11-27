set(CXXBRIDGE_BINARY ${CMAKE_CURRENT_BINARY_DIR}/cargo/bin/cxxbridge)
if (NOT EXISTS ${CXXBRIDGE_BINARY})
    message(STATUS "Fetching rust cxxbridge cli tool")
    execute_process(
        COMMAND
            cargo install cxxbridge-cmd --root ${CMAKE_CURRENT_BINARY_DIR}/cargo
        WORKING_DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR}
        RESULT_VARIABLE
            CARGO_INSTALL_RESULT
        OUTPUT_QUIET
        ERROR_VARIABLE
            CARGO_INSTALL_ERROR
    )

    if (CARGO_INSTALL_RESULT)
        message(FATAL_ERROR "cargo install cxxbridge-cmd failed with:\n${CARGO_INSTALL_ERROR}")
    endif ()
endif ()

set(CXXBRIDGE_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/cxxbridge)
set(EVERESTRS_FFI_HEADER ${CXXBRIDGE_OUTPUT_DIR}/lib.rs.h)
set(EVERESTRS_FFI_SOURCE ${CXXBRIDGE_OUTPUT_DIR}/lib.rs.cc)
set(CXXBRIDGE_HEADER ${CXXBRIDGE_OUTPUT_DIR}/rust.h)

add_custom_command(
    OUTPUT
        ${CXXBRIDGE_OUTPUT_DIR}
        ${EVERESTRS_FFI_HEADER}
        ${EVERESTRS_FFI_SOURCE}
        ${CXXBRIDGE_HEADER}
    COMMAND
        ${CMAKE_COMMAND} -E make_directory ${CXXBRIDGE_OUTPUT_DIR}
    COMMAND
        ${CXXBRIDGE_BINARY} --header -o ${CXXBRIDGE_HEADER}
    COMMAND
        ${CXXBRIDGE_BINARY} ${CMAKE_CURRENT_SOURCE_DIR}/everestrs/src/lib.rs --header -o ${EVERESTRS_FFI_HEADER}
    COMMAND
        ${CXXBRIDGE_BINARY} ${CMAKE_CURRENT_SOURCE_DIR}/everestrs/src/lib.rs -o ${EVERESTRS_FFI_SOURCE}
    DEPENDS
        everestrs/src/lib.rs
    COMMENT "Generating cxxbridge glue for everestrs"
    VERBATIM
    DEPENDS
        ${CXXBRIDGE_BINARY}
)

add_library(everestrs_sys STATIC
    ${EVERESTRS_FFI_SOURCE}
    everestrs_sys/everestrs_sys.cpp
)
add_library(everest::everestrs_sys ALIAS everestrs_sys)

set_property(
    TARGET
        everestrs_sys
    PROPERTY
        EVERESTRS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/everestrs
)

set_property(
    TARGET
        everestrs_sys
    PROPERTY
        EVERESTRS_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/everestrs-build
)

target_include_directories(everestrs_sys
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)

# This is a requirement that linking works on systems enforcing PIE
# FIXME (aw): investicate why this is really necessary
set_property(TARGET everestrs_sys PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(everestrs_sys
    PRIVATE
        everest::framework
        everest::log
)

install(TARGETS everestrs_sys LIBRARY)
