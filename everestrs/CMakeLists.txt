include(cxxrs)

find_program(CXXBRIDGE cxxbridge PATHS "$ENV{HOME}/.cargo/bin/")
if (CXXBRIDGE STREQUAL "CXXBRIDGE-NOTFOUND")
  message("Could not find cxxbridge, trying to install with `cargo install cxxbridge-cmd'")
  find_program(CARGO cargo PATHS "$ENV{HOME}/.cargo/bin/")
  if (CARGO STREQUAL "CARGO-NOTFOUND")
    message(FATAL_ERROR "Requires cargo available in path, install via rustup https://rustup.rs/")
  endif()
  execute_process(COMMAND ${CARGO} install cxxbridge-cmd --version 1.0.107)
  find_program(CXXBRIDGE cxxbridge PATHS "$ENV{HOME}/.cargo/bin/")
endif()

emit_cxxrs_header()
emit_cxxrs_for_module(everestrs)

add_library(everestrs_sys STATIC
  ${CMAKE_CURRENT_BINARY_DIR}/cxxbridge/everestrs/lib.rs.cc
  everestrs_sys/everestrs_sys.cpp
)

target_include_directories(everestrs_sys PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}/cxxbridge
)

# This is a requirement that linking works on systems enforcing PIE.
set_property(TARGET everestrs_sys PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(everestrs_sys
    PRIVATE
    everest::framework
    everest::log
)

install(TARGETS everestrs_sys LIBRARY)
