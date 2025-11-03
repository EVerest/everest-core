file(GLOB_RECURSE MODBUS_CLIENT_SOURCES "src/*.cpp")

# Create modbus-client library
add_library(
    modbus-client
    STATIC
        ${MODBUS_CLIENT_SOURCES}
)
target_include_directories(modbus-client PUBLIC include)
target_link_libraries(modbus-client PUBLIC modbus-base)

if(EVEREST_CORE_BUILD_TESTING)
    add_subdirectory(tests)
endif()
