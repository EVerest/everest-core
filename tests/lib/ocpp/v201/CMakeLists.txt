add_executable(device_model_storage_sqlite_tests test_device_model_storage_sqlite.cpp
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../../lib/ocpp/v201/device_model_storage_sqlite.cpp")

target_include_directories(device_model_storage_sqlite_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/../../../../include" ${GTEST_INCLUDE_DIRS})

find_package(GTest REQUIRED)

target_link_libraries(device_model_storage_sqlite_tests PRIVATE
        GTest::gmock_main
        everest::timer
        everest::evse_security # since it contains the everest-logging dependency; to be improved
        SQLite::SQLite3
        nlohmann_json::nlohmann_json
        ${GTEST_LIBRARIES}
        ${GTEST_MAIN_LIBRARIES}
)

add_custom_command(TARGET device_model_storage_sqlite_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/unittest_device_model.db ${CMAKE_CURRENT_BINARY_DIR}/unittest_device_model.db
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/unittest_device_model_missing_required.db ${CMAKE_CURRENT_BINARY_DIR}/unittest_device_model_missing_required.db
)

add_test(device_model_storage_sqlite_tests device_model_storage_sqlite_tests)
set_tests_properties(device_model_storage_sqlite_tests PROPERTIES ENVIRONMENT "DATADIR=blabla")
set_tests_properties(device_model_storage_sqlite_tests PROPERTIES
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)