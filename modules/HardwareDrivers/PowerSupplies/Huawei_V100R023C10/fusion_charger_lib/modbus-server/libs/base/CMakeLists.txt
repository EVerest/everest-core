file(GLOB_RECURSE MODBUS_BASE_SOURCES "src/*.cpp")

# Create modbus-base library
add_library(
    modbus-base
    STATIC
        ${MODBUS_BASE_SOURCES}
)
target_include_directories(modbus-base PUBLIC include)

if(FUSION_CHARGER_LIB_BUILD_TESTS)
    add_subdirectory(tests)
endif()
