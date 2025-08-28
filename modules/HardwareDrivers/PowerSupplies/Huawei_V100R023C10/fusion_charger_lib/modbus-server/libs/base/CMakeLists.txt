file(GLOB_RECURSE MODBUS_BASE_SOURCES "src/*.cpp")

# Create modbus-base library
add_library(
    modbus-base
    STATIC
        ${MODBUS_BASE_SOURCES}
)
target_include_directories(modbus-base PUBLIC include)

if(BUILD_TESTING)
    if(TEST_COVERAGE)
        # Add coverage flags
        target_compile_options(modbus-base PUBLIC --coverage)
        target_link_options(modbus-base PUBLIC --coverage)
    endif()
    add_subdirectory(tests)
endif()
