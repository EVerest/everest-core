target_include_directories(libocpp_unit_tests PUBLIC 
        ../mocks
        ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(libocpp_unit_tests PRIVATE
        test_data_transfer.cpp
        test_reservation.cpp)


set(TEST_AUTHORIZATION_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../device_model_test_helper.cpp
            ${LIBOCPP_LIB_PATH}/ocpp/v201/functional_blocks/authorization.cpp
            ${LIBOCPP_LIB_PATH}/ocpp/v201/messages/Authorize.cpp
            ${LIBOCPP_LIB_PATH}/ocpp/v201/messages/ClearCache.cpp
            ${LIBOCPP_LIB_PATH}/ocpp/v201/messages/GetLocalListVersion.cpp
            ${LIBOCPP_LIB_PATH}/ocpp/v201/messages/SendLocalList.cpp
            ${LIBOCPP_TEST_INCLUDE_COMMON_SOURCES}
            ${LIBOCPP_TEST_INCLUDE_V201_SOURCES}
)

target_sources(libocpp_test_authorization PRIVATE
        ${TEST_AUTHORIZATION_SOURCES})

target_include_directories(libocpp_test_authorization PUBLIC
        ${LIBOCPP_INCLUDE_PATH}
        ${LIBOCPP_3RDPARTY_PATH}
        ${CMAKE_CURRENT_SOURCE_DIR}/../mocks
        ${LIBOCPP_TESTS_V201_ROOT_DIR}
)
