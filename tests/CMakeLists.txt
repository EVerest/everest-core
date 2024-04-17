set(MIGRATION_FILES_LOCATION_V16 "${CMAKE_CURRENT_BINARY_DIR}/resources/v16/migration_files")
set(MIGRATION_FILES_LOCATION_V201 "${CMAKE_CURRENT_BINARY_DIR}/resources/v201/migration_files")

add_executable(database_tests database_tests.cpp)

target_link_libraries(database_tests PRIVATE
    ocpp
    date::date
    nlohmann_json::nlohmann_json
    nlohmann_json_schema_validator
    GTest::gtest_main
    pthread
    )


add_test(database_tests database_tests)

add_executable(libocpp_unit_tests)

target_compile_definitions(database_tests
    PRIVATE
    MIGRATION_FILES_LOCATION_V16="${MIGRATION_FILES_LOCATION_V16}"
    MIGRATION_FILES_LOCATION_V201="${MIGRATION_FILES_LOCATION_V201}"
    MIGRATION_FILE_VERSION_V16=${MIGRATION_FILE_VERSION_V16}
    MIGRATION_FILE_VERSION_V201=${MIGRATION_FILE_VERSION_V201}
)

target_compile_definitions(libocpp_unit_tests
    PRIVATE
    MIGRATION_FILES_LOCATION_V16="${MIGRATION_FILES_LOCATION_V16}"
    MIGRATION_FILES_LOCATION_V201="${MIGRATION_FILES_LOCATION_V201}"
    MIGRATION_FILE_VERSION_V16=${MIGRATION_FILE_VERSION_V16}
    MIGRATION_FILE_VERSION_V201=${MIGRATION_FILE_VERSION_V201}
)

add_custom_command(TARGET libocpp_unit_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/resources/unittest_device_model.db ${CMAKE_CURRENT_BINARY_DIR}/resources/unittest_device_model.db
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/resources/unittest_device_model_missing_required.db ${CMAKE_CURRENT_BINARY_DIR}/resources/unittest_device_model_missing_required.db
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${MIGRATION_FILES_LOCATION_V16}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${MIGRATION_FILES_SOURCE_DIR_V16} ${MIGRATION_FILES_LOCATION_V16}
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${MIGRATION_FILES_LOCATION_V201}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${MIGRATION_FILES_SOURCE_DIR_V201} ${MIGRATION_FILES_LOCATION_V201}
)

add_test(libocpp_unit_tests libocpp_unit_tests)

target_include_directories(libocpp_unit_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(libocpp_unit_tests PRIVATE
        ocpp
        GTest::gmock_main
        GTest::gtest_main
)
target_sources(libocpp_unit_tests PRIVATE
        comparators.cpp)

add_subdirectory(lib/ocpp/v16)
add_subdirectory(lib/ocpp/v201)
add_subdirectory(lib/ocpp/common)


add_executable(utils_tests utils_tests.cpp)

target_link_libraries(utils_tests PRIVATE
        ocpp
        GTest::gtest_main
)

add_test(utils_tests utils_tests)

set(TEST_TARGET_NAME ${PROJECT_NAME}_v201_utils_tests)
add_executable(${TEST_TARGET_NAME} v201_utils_tests.cpp)

target_link_libraries(${TEST_TARGET_NAME} PRIVATE
        ocpp
        GTest::gtest_main
)

add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})
