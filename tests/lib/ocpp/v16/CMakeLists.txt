target_include_directories(libocpp_unit_tests PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(libocpp_unit_tests PRIVATE
        profile_tests_common.cpp
        profile_testsA.cpp
        profile_testsB.cpp
        profile_testsC.cpp
        test_database_migration_files.cpp
        test_smart_charging_handler.cpp
        database_tests.cpp
        test_message_queue.cpp
        test_charge_point_state_machine.cpp
)
