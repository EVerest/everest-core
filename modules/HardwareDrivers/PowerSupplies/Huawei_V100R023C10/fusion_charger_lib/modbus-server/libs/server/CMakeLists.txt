file(GLOB_RECURSE MODBUS_SERVER_SOURCES "src/*.cpp")

add_library(
    modbus-server
    STATIC
        ${MODBUS_SERVER_SOURCES}
)
target_include_directories(modbus-server PUBLIC include)
target_link_libraries(modbus-server PUBLIC modbus-base Huawei::FusionCharger::LogInterface)

if(FUSION_CHARGER_LIB_BUILD_TESTS)
    add_subdirectory(tests)
endif()
