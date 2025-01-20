find_package(OpenSSL 3)

set(TLS_TEST_FILES
        alt_openssl-pki.conf
        iso_pkey.asn1
        openssl-pki.conf
        ocsp_response.der
        pki.sh
)

add_custom_command(
    OUTPUT ${TLS_TEST_FILES}
    COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/pki
    COMMAND cd pki && cp ${TLS_TEST_FILES} ${CMAKE_CURRENT_BINARY_DIR}/
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(tls_test_files_target
    DEPENDS ${TLS_TEST_FILES}
)

set(TLS_GTEST_NAME tls_test)
add_executable(${TLS_GTEST_NAME})
add_dependencies(${TLS_GTEST_NAME} tls_test_files_target)

target_include_directories(${TLS_GTEST_NAME} PRIVATE
    ..)

target_compile_definitions(${TLS_GTEST_NAME} PRIVATE
    -DUNIT_TEST
    -DLIBEVSE_CRYPTO_SUPPLIER_OPENSSL
)

target_sources(${TLS_GTEST_NAME} PRIVATE
    gtest_main.cpp
    crypto_test.cpp
    openssl_util_test.cpp
    tls_test.cpp
    tls_connection_test.cpp
    ../extensions/helpers.cpp
    ../extensions/status_request.cpp
    ../extensions/trusted_ca_keys.cpp
    ../openssl_conv.cpp
    ../openssl_util.cpp
    ../tls.cpp
)

target_link_libraries(${TLS_GTEST_NAME}
    PRIVATE
        GTest::gtest
        OpenSSL::SSL
        OpenSSL::Crypto
        everest::evse_security
        everest::staging::util
)

set(TLS_MAIN_NAME tls_server)
add_executable(${TLS_MAIN_NAME})
add_dependencies(${TLS_MAIN_NAME} tls_test_files_target)

target_include_directories(${TLS_MAIN_NAME} PRIVATE
    ..
)

target_compile_definitions(${TLS_MAIN_NAME} PRIVATE
    -DUNIT_TEST
)

target_sources(${TLS_MAIN_NAME} PRIVATE
    tls_main.cpp
    ../extensions/helpers.cpp
    ../extensions/status_request.cpp
    ../extensions/trusted_ca_keys.cpp
    ../openssl_util.cpp
    ../tls.cpp
)

target_link_libraries(${TLS_MAIN_NAME}
    PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
        everest::staging::util
)

set(TLS_CLIENT_NAME tls_client)
add_executable(${TLS_CLIENT_NAME})
add_dependencies(${TLS_CLIENT_NAME} tls_test_files_target)

target_include_directories(${TLS_CLIENT_NAME} PRIVATE
    ..
)

target_compile_definitions(${TLS_CLIENT_NAME} PRIVATE
    -DUNIT_TEST
)

target_sources(${TLS_CLIENT_NAME} PRIVATE
    tls_client_main.cpp
    ../extensions/helpers.cpp
    ../extensions/status_request.cpp
    ../extensions/trusted_ca_keys.cpp
    ../openssl_util.cpp
    ../tls.cpp
)

target_link_libraries(${TLS_CLIENT_NAME}
    PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
        everest::staging::util
)

set(TLS_PATCH_NAME patched_test)
add_executable(${TLS_PATCH_NAME})
add_dependencies(${TLS_PATCH_NAME} tls_test_files_target)

target_include_directories(${TLS_PATCH_NAME} PRIVATE
    ..
)

target_compile_definitions(${TLS_PATCH_NAME} PRIVATE
    -DUNIT_TEST
)

target_sources(${TLS_PATCH_NAME} PRIVATE
    patched_test.cpp
    ../extensions/helpers.cpp
    ../extensions/status_request.cpp
    ../extensions/trusted_ca_keys.cpp
    ../openssl_util.cpp
    ../tls.cpp
)

target_link_libraries(${TLS_PATCH_NAME}
    PRIVATE
        GTest::gtest_main
        OpenSSL::SSL
        OpenSSL::Crypto
        everest::staging::util
)

add_test(${TLS_GTEST_NAME} ${TLS_GTEST_NAME})
ev_register_test_target(${TLS_GTEST_NAME})
