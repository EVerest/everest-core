set(CXXBRIDGE_VERSION 1.0.189) # Must be kept in sync with `everestrs/Cargo.toml`
set(CXXBRIDGE_INSTALL_PATH ${CMAKE_CURRENT_BINARY_DIR}/cargo/bin/cxxbridge)

# Checks if the cxxbridge binary at CXXBRIDGE_PATH exists and matches the required version.
# If not, sets VERSION_MATCHES to FALSE.
function(everestrs_check_cxxbridge_version CXXBRIDGE_PATH VERSION_MATCHES)
    if(NOT CXXBRIDGE_BINARY OR NOT EXISTS ${CXXBRIDGE_BINARY})
        set(${VERSION_MATCHES} FALSE PARENT_SCOPE)
    endif()

    execute_process(
        COMMAND
            ${CXXBRIDGE_BINARY} --version
        WORKING_DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR}
        OUTPUT_VARIABLE
            CXXBRIDGE_VERSION_OUTPUT
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(CXXBRIDGE_VERSION_OUTPUT MATCHES "cxxbridge ${CXXBRIDGE_VERSION}")
        set(${VERSION_MATCHES} TRUE PARENT_SCOPE)
    else()
        set(${VERSION_MATCHES} FALSE PARENT_SCOPE)
    endif()
endfunction()

# If cxxbridge is already installed system-wide, check if it's the correct version.
find_program(CXXBRIDGE_BINARY cxxbridge)
everestrs_check_cxxbridge_version(${CXXBRIDGE_BINARY} CXXBRIDGE_INSTALLED)

# Otherwise, check if we previously installed it in our build dir.
if(NOT CXXBRIDGE_INSTALLED)
    set(CXXBRIDGE_BINARY ${CXXBRIDGE_INSTALL_PATH})
    everestrs_check_cxxbridge_version(${CXXBRIDGE_BINARY} CXXBRIDGE_INSTALLED)
endif()

# Either we never had cxxbridge installed, or the version is incorrect. Install the correct version to our build dir.
if(NOT CXXBRIDGE_INSTALLED)
    message(STATUS "Fetching rust cxxbridge cli tool")
    execute_process(
        COMMAND
            ${CMAKE_COMMAND} -E env --unset=CARGO_BUILD_TARGET # Ensure we always build for the host
            cargo install cxxbridge-cmd --force --version ${CXXBRIDGE_VERSION} --root ${CMAKE_CURRENT_BINARY_DIR}/cargo
        WORKING_DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR}
        RESULT_VARIABLE
            CARGO_INSTALL_RESULT
        OUTPUT_QUIET
        ERROR_VARIABLE
            CARGO_INSTALL_ERROR
    )

    if(NOT CARGO_INSTALL_RESULT)
        set(CXXBRIDGE_BINARY ${CXXBRIDGE_INSTALL_PATH})
    else()
        message(FATAL_ERROR "cargo install cxxbridge-cmd failed with:\n${CARGO_INSTALL_ERROR}")
    endif()
endif()

set(CXXBRIDGE_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(CXXBRIDGE_HEADER ${CXXBRIDGE_OUTPUT_DIR}/rust/cxx.h)
set(EVERESTRS_FFI_HEADER ${CXXBRIDGE_OUTPUT_DIR}/everestrs/src/lib.rs.h)
set(EVERESTRS_FFI_SOURCE ${CXXBRIDGE_OUTPUT_DIR}/everestrs/src/lib.rs.cc)

add_custom_command(
    OUTPUT
        ${EVERESTRS_FFI_HEADER}
        ${EVERESTRS_FFI_SOURCE}
        ${CXXBRIDGE_HEADER}
    COMMAND
        ${CMAKE_COMMAND} -E make_directory ${CXXBRIDGE_OUTPUT_DIR}/everestrs/src ${CXXBRIDGE_OUTPUT_DIR}/rust
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
    everestrs/src/everestrs_sys.cpp
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

set(EVERESTRS_LINK_DEPENDENCIES $<TARGET_FILE:everest::everestrs_sys>)
get_target_property(EVERESTRS_LINK_TARGETS everest::everestrs_sys LINK_LIBRARIES)
foreach(link_target IN LISTS EVERESTRS_LINK_TARGETS)
    list(APPEND EVERESTRS_LINK_DEPENDENCIES $<TARGET_FILE:${link_target}>)
endforeach()

set_property(
    TARGET
        everestrs_sys
    PROPERTY
        EVERESTRS_LINK_DEPENDENCIES "${EVERESTRS_LINK_DEPENDENCIES}"
)
