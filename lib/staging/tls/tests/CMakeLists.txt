find_package(OpenSSL 3)

set(TLS_GTEST_NAME tls_test)
add_executable(${TLS_GTEST_NAME})

target_include_directories(${TLS_GTEST_NAME} PRIVATE
    . .. ../../util
)

target_compile_definitions(${TLS_GTEST_NAME} PRIVATE
    -DUNIT_TEST
)

target_sources(${TLS_GTEST_NAME} PRIVATE
    gtest_main.cpp
    crypto_test.cpp
    openssl_util_test.cpp
    tls_test.cpp
    ../openssl_conv.cpp
    ../openssl_util.cpp
    ../tls.cpp
)

target_link_libraries(${TLS_GTEST_NAME} PRIVATE
    GTest::gtest
    OpenSSL::SSL
    OpenSSL::Crypto
    everest::evse_security
)

set(TLS_MAIN_NAME tls_server)
add_executable(${TLS_MAIN_NAME})

target_include_directories(${TLS_MAIN_NAME} PRIVATE
    . .. ../../util
)

target_compile_definitions(${TLS_MAIN_NAME} PRIVATE
    -DUNIT_TEST
)

target_sources(${TLS_MAIN_NAME} PRIVATE
    tls_main.cpp
    ../openssl_util.cpp
    ../tls.cpp
)

target_link_libraries(${TLS_MAIN_NAME} PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
)

set(TLS_CLIENT_NAME tls_client)
add_executable(${TLS_CLIENT_NAME})

target_include_directories(${TLS_CLIENT_NAME} PRIVATE
    . .. ../../util
)

target_compile_definitions(${TLS_CLIENT_NAME} PRIVATE
    -DUNIT_TEST
)

target_sources(${TLS_CLIENT_NAME} PRIVATE
    tls_client_main.cpp
    ../openssl_util.cpp
    ../tls.cpp
)

target_link_libraries(${TLS_CLIENT_NAME} PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
)

set(TLS_PATCH_NAME patched_test)
add_executable(${TLS_PATCH_NAME})

target_include_directories(${TLS_PATCH_NAME} PRIVATE
    . .. ../../util
)

target_compile_definitions(${TLS_PATCH_NAME} PRIVATE
    -DUNIT_TEST
)

target_sources(${TLS_PATCH_NAME} PRIVATE
    patched_test.cpp
    ../openssl_util.cpp
    ../tls.cpp
)

target_link_libraries(${TLS_PATCH_NAME} PRIVATE
    GTest::gtest_main
    OpenSSL::SSL
    OpenSSL::Crypto
)

install(
    FILES
    pki/iso_pkey.asn1
    pki/openssl-pki.conf
    pki/ocsp_response.der
    DESTINATION "${CMAKE_CURRENT_BINARY_DIR}"
)

install(
    PROGRAMS
    pki/pki.sh
    DESTINATION "${CMAKE_CURRENT_BINARY_DIR}"
)

# tests don't run successfully in CI pipeline
# pki.sh not installed or run from wrong directory
# add_test(${TLS_GTEST_NAME} ${TLS_GTEST_NAME})
