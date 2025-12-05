file(GLOB_RECURSE MODBUS_CLIENT_SOURCES "src/*.cpp")

# Create modbus-client library
add_library(
    modbus-client
    STATIC
        ${MODBUS_CLIENT_SOURCES}
)
target_include_directories(modbus-client PUBLIC include)
target_link_libraries(modbus-client PUBLIC modbus-base)

if(BUILD_TESTING)
    if(TEST_COVERAGE)
        # Add coverage flags
        target_compile_options(modbus-client PUBLIC --coverage)
        target_link_options(modbus-client PUBLIC --coverage)
    endif()
    add_subdirectory(tests)
endif()
