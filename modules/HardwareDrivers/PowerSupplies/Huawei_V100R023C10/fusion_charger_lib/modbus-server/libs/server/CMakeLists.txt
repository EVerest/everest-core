file(GLOB_RECURSE MODBUS_SERVER_SOURCES "src/*.cpp")

add_library(
    modbus-server
    STATIC
        ${MODBUS_SERVER_SOURCES}
)
target_include_directories(modbus-server PUBLIC include)
target_link_libraries(modbus-server PUBLIC modbus-base Huawei::FusionCharger::LogInterface)

if(EVEREST_CORE_BUILD_TESTING)
    add_subdirectory(tests)
endif()
