file(GLOB_RECURSE LIB_SOURCES "lib/*.cpp")
add_library(power_stack_mock_lib STATIC ${LIB_SOURCES})
target_include_directories(power_stack_mock_lib PUBLIC include)

target_link_libraries(power_stack_mock_lib
    PUBLIC
    fusion_charger_dispenser
    modbus-ssl
    modbus-client
    atomic
)

file (GLOB_RECURSE EXECUTABLE_SOURCES "src/*.cpp")

add_executable(fusion_charger_mock ${EXECUTABLE_SOURCES})
target_link_libraries(fusion_charger_mock power_stack_mock_lib)
target_link_libraries(fusion_charger_mock atomic mqttc)
