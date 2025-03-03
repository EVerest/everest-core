target_include_directories(libocpp_unit_tests PUBLIC
        ../mocks
        ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(libocpp_unit_tests PRIVATE
        test_data_transfer.cpp
        test_reservation.cpp
        test_smart_charging.cpp)


set(TEST_FUNCTIONAL_BLOCK_CONTEXT_SOURCES ${LIBOCPP_LIB_PATH}/ocpp/v2/average_meter_values.cpp
                                          ${LIBOCPP_LIB_PATH}/ocpp/v2/component_state_manager.cpp
                                          ${LIBOCPP_LIB_PATH}/ocpp/v2/connector.cpp
                                          ${LIBOCPP_LIB_PATH}/ocpp/v2/database_handler.cpp
                                          ${LIBOCPP_LIB_PATH}/ocpp/v2/evse.cpp
                                          ${LIBOCPP_LIB_PATH}/ocpp/v2/transaction.cpp)


set(TEST_SECURITY_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../device_model_test_helper.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/../stubs/timer/timer_stub.cpp
            ${TEST_FUNCTIONAL_BLOCK_CONTEXT_SOURCES}
            ${LIBOCPP_LIB_PATH}/ocpp/v2/functional_blocks/security.cpp
            ${LIBOCPP_LIB_PATH}/ocpp/v2/messages/SecurityEventNotification.cpp
            ${LIBOCPP_LIB_PATH}/ocpp/v2/messages/CertificateSigned.cpp
            ${LIBOCPP_LIB_PATH}/ocpp/v2/messages/DeleteCertificate.cpp
            ${LIBOCPP_LIB_PATH}/ocpp/v2/messages/GetInstalledCertificateIds.cpp
            ${LIBOCPP_LIB_PATH}/ocpp/v2/messages/InstallCertificate.cpp
            ${LIBOCPP_LIB_PATH}/ocpp/v2/messages/SignCertificate.cpp
            ${LIBOCPP_LIB_PATH}/ocpp/v2/messages/Get15118EVCertificate.cpp
            ${LIBOCPP_LIB_PATH}/ocpp/v2/messages/Reset.cpp
            ${LIBOCPP_TEST_INCLUDE_COMMON_SOURCES}
            ${LIBOCPP_TEST_INCLUDE_V2_SOURCES}
)

target_sources(libocpp_test_security PRIVATE
        ${TEST_SECURITY_SOURCES})

target_include_directories(libocpp_test_security PUBLIC
        ${LIBOCPP_INCLUDE_PATH}
        ${LIBOCPP_3RDPARTY_PATH}
        ${CMAKE_CURRENT_SOURCE_DIR}/../stubs/timer
        ${CMAKE_CURRENT_SOURCE_DIR}/../mocks
        ${LIBOCPP_TESTS_V2_ROOT_DIR}
)


set(TEST_AUTHORIZATION_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../device_model_test_helper.cpp
            ${TEST_FUNCTIONAL_BLOCK_CONTEXT_SOURCES}
            ${LIBOCPP_LIB_PATH}/ocpp/v2/functional_blocks/authorization.cpp
            ${LIBOCPP_LIB_PATH}/ocpp/v2/messages/Authorize.cpp
            ${LIBOCPP_LIB_PATH}/ocpp/v2/messages/ClearCache.cpp
            ${LIBOCPP_LIB_PATH}/ocpp/v2/messages/GetLocalListVersion.cpp
            ${LIBOCPP_LIB_PATH}/ocpp/v2/messages/SendLocalList.cpp
            ${LIBOCPP_TEST_INCLUDE_COMMON_SOURCES}
            ${LIBOCPP_TEST_INCLUDE_V2_SOURCES}
)

target_sources(libocpp_test_authorization PRIVATE
        ${TEST_AUTHORIZATION_SOURCES})

target_include_directories(libocpp_test_authorization PUBLIC
        ${LIBOCPP_INCLUDE_PATH}
        ${LIBOCPP_3RDPARTY_PATH}
        ${CMAKE_CURRENT_SOURCE_DIR}/../mocks
        ${LIBOCPP_TESTS_V2_ROOT_DIR}
)
