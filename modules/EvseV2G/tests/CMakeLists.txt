get_target_property(GENERATED_INCLUDE_DIR generate_cpp_files EVEREST_GENERATED_INCLUDE_DIR)
find_package(libevent)

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
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}../../../../lib/staging/tls/tests/
)

add_custom_target(v2g_test_files_target
    DEPENDS ${TLS_TEST_FILES}
)

set(TLS_GTEST_NAME v2g_openssl_test)
add_executable(${TLS_GTEST_NAME})
add_dependencies(${TLS_GTEST_NAME} v2g_test_files_target)

add_dependencies(${TLS_GTEST_NAME} generate_cpp_files)

target_include_directories(${TLS_GTEST_NAME} PRIVATE
    .. ../crypto
    ${GENERATED_INCLUDE_DIR}
    ${CMAKE_BINARY_DIR}/generated/modules/${MODULE_NAME}
)

target_compile_definitions(${TLS_GTEST_NAME} PRIVATE
    -DUNIT_TEST
)

target_sources(${TLS_GTEST_NAME} PRIVATE
    ../../../lib/staging/tls/tests/gtest_main.cpp
    log.cpp
    openssl_test.cpp
    ../crypto/crypto_openssl.cpp
)

target_link_libraries(${TLS_GTEST_NAME} PRIVATE
    GTest::gtest
    cbv2g::din
    cbv2g::iso2
    cbv2g::tp
    everest::framework
    everest::evse_security
    everest::tls
)

set(V2G_MAIN_NAME v2g_server)
add_executable(${V2G_MAIN_NAME})

add_dependencies(${V2G_MAIN_NAME} generate_cpp_files)

target_include_directories(${V2G_MAIN_NAME} PRIVATE
    .. ../connection ../../../tests/include
    ${GENERATED_INCLUDE_DIR}
    ${CMAKE_BINARY_DIR}/generated/modules/${MODULE_NAME}
    ${CMAKE_BINARY_DIR}/generated/include
)

target_compile_definitions(${V2G_MAIN_NAME} PRIVATE
    -DUNIT_TEST
)

target_sources(${V2G_MAIN_NAME} PRIVATE
    ../connection/connection.cpp
    ../connection/tls_connection.cpp
    ../tools.cpp
    ../v2g_ctx.cpp
    log.cpp
    requirement.cpp
    v2g_main.cpp
)

target_link_libraries(${V2G_MAIN_NAME} PRIVATE
    cbv2g::din
    cbv2g::iso2
    cbv2g::tp
    everest::log
    everest::framework
    everest::evse_security
    everest::tls
    -levent -lpthread -levent_pthreads
)

# runs fine locally, fails in CI
add_test(${TLS_GTEST_NAME} ${TLS_GTEST_NAME})
