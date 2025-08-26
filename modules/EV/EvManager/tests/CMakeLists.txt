set(TEST_TARGET_NAME ${PROJECT_NAME}_EvManager_tests)
add_executable(${TEST_TARGET_NAME})

add_dependencies(${TEST_TARGET_NAME} ${MODULE_NAME})

get_target_property(GENERATED_INCLUDE_DIR generate_cpp_files EVEREST_GENERATED_INCLUDE_DIR)
target_include_directories(${TEST_TARGET_NAME}
    PRIVATE
        "$<TARGET_PROPERTY:generate_cpp_files,EVEREST_GENERATED_INCLUDE_DIR>"
)

target_sources(${TEST_TARGET_NAME}
    PRIVATE
        CommandRegistryTest.cpp
        SimCommandTest.cpp
        ../main/simulation_command.cpp
)

target_compile_definitions(${TEST_TARGET_NAME} PRIVATE
    BUILD_TESTING_MODULE_EV_MANAGER
)

target_link_libraries(${TEST_TARGET_NAME}
    PRIVATE
        everest::framework
        everest::log
        Catch2::Catch2WithMain
)

if (NOT DISABLE_EDM)
    list(APPEND CMAKE_MODULE_PATH ${CPM_PACKAGE_catch2_SOURCE_DIR}/extras)
    include(Catch)
    catch_discover_tests(${TEST_TARGET_NAME})
endif ()

add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})
ev_register_test_target(${TEST_TARGET_NAME})
