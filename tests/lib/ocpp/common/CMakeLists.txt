target_sources(libocpp_unit_tests PRIVATE
    test_database_migration_files.cpp
    test_database_schema_updater.cpp
    test_message_queue.cpp
    test_websocket_uri.cpp
)


set(TEST_UTILS_SOURCES ${LIBOCPP_LIB_PATH}/ocpp/common/utils.cpp)

target_sources(libocpp_utils_tests PRIVATE ${TEST_UTILS_SOURCES})
