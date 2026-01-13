target_include_directories(libocpp_unit_tests PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(libocpp_unit_tests PRIVATE
        v2config/memory_storage.cpp
        v2config/test_california_pricing.cpp
        v2config/test_config.cpp
        v2config/test_core.cpp
        v2config/test_custom.cpp
        v2config/test_firmware_management.cpp
        v2config/test_internal.cpp
        v2config/test_local_auth_list.cpp
        v2config/test_pnc.cpp
        v2config/test_security.cpp
        v2config/test_smart_charging.cpp
        v2config/test_user_config.cpp
        profile_tests_common.cpp
        profile_testsA.cpp
        profile_testsB.cpp
        profile_testsC.cpp
        test_database_migration_files.cpp
        test_smart_charging_handler.cpp
        database_tests.cpp
        test_message_queue.cpp
        test_charge_point_state_machine.cpp
        test_composite_schedule.cpp
        test_config_validation.cpp
        utils_tests.cpp
        test_configuration.cpp
        test_utils.cpp
)

# Copy the json files used for testing to the destination directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/json DESTINATION ${TEST_PROFILES_LOCATION_V16})
