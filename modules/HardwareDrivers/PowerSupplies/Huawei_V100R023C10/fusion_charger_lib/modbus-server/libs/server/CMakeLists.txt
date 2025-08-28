file(GLOB_RECURSE MODBUS_SERVER_SOURCES "src/*.cpp")

add_library(
    modbus-server
    STATIC
        ${MODBUS_SERVER_SOURCES}
)
target_include_directories(modbus-server PUBLIC include)
target_link_libraries(modbus-server PUBLIC modbus-base Huawei::FusionCharger::LogInterface)

if(BUILD_TESTING)
    if(TEST_COVERAGE)
        # Add coverage flags
        target_compile_options(modbus-server PUBLIC --coverage)
        target_link_options(modbus-server PUBLIC --coverage)
    endif()
    add_subdirectory(tests)
endif()
