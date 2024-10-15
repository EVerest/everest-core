file(GLOB_RECURSE MODBUS_CLIENT_SOURCES "src/*.cpp")

# Create modbus-client library
add_library(
    modbus-client
    STATIC
        ${MODBUS_CLIENT_SOURCES}
)
target_include_directories(modbus-client PUBLIC include)
target_link_libraries(modbus-client PUBLIC modbus-base)
ev_register_library_target(modbus-client)
