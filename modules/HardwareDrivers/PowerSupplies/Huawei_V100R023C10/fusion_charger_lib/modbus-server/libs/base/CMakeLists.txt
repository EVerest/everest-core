file(GLOB_RECURSE MODBUS_BASE_SOURCES "src/*.cpp")

# Create modbus-base library
add_library(
    modbus-base
    STATIC
        ${MODBUS_BASE_SOURCES}
)
target_include_directories(modbus-base PUBLIC include)

if(EVEREST_CORE_BUILD_TESTING)
    add_subdirectory(tests)
endif()
