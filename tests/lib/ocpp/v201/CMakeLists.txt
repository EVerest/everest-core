target_include_directories(libocpp_unit_tests PUBLIC mocks)

target_sources(libocpp_unit_tests PRIVATE
        test_device_model_storage_sqlite.cpp
        test_notify_report_requests_splitter.cpp
        test_ocsp_updater.cpp
        test_component_state_manager.cpp
        test_device_model.cpp
        test_smart_charging_handler.cpp)
