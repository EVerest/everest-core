target_include_directories(libocpp_unit_tests PUBLIC 
        mocks
        ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(libocpp_unit_tests PRIVATE
        test_charge_point.cpp
        test_database_handler.cpp
        test_database_migration_files.cpp
        test_device_model_storage_sqlite.cpp
        test_notify_report_requests_splitter.cpp
        test_ocsp_updater.cpp
        test_component_state_manager.cpp
        test_database_handler.cpp
        test_device_model.cpp
        test_init_device_model_db.cpp
        test_smart_charging_handler.cpp
        utils_tests.cpp
        comparators.cpp
        test_message_queue.cpp)
