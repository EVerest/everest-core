set(TEST_TARGET_NAME ${PROJECT_NAME}_tests)
add_executable(${TEST_TARGET_NAME})

target_sources(${TEST_TARGET_NAME} PRIVATE
    tests.cpp
    openssl_supplier_test.cpp
)

find_package(OpenSSL REQUIRED)

target_link_libraries(${TEST_TARGET_NAME} PRIVATE
    evse_security
    GTest::gtest_main
)

if(USING_TPM2)
    target_sources(${TEST_TARGET_NAME} PRIVATE
        openssl_supplier_test_tpm.cpp
    )
    target_compile_definitions(${TEST_TARGET_NAME} PRIVATE
        USING_TPM2
        PROPQUERY_DEFAULT="${PROPQUERY_DEFAULT}"
        PROPQUERY_TPM2="${PROPQUERY_TPM2}"
    )
endif()

if(LIBEVSE_CRYPTO_SUPPLIER_OPENSSL)
    add_compile_definitions(LIBEVSE_CRYPTO_SUPPLIER_OPENSSL)
endif()

add_compile_definitions(BUILD_TESTING_EVSE_SECURITY)

add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})

install(
    PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/generate_test_certs.sh"
    PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/generate_test_certs_multi.sh"
    DESTINATION "${CMAKE_BINARY_DIR}/tests"
)

install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/configs"
    DESTINATION "${CMAKE_BINARY_DIR}/tests"
    FILES_MATCHING PATTERN "*.cnf"
)

install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/future_leaf"
    DESTINATION "${CMAKE_BINARY_DIR}/tests"
    FILES_MATCHING PATTERN "*"
)

install(
        DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/expired_leaf"
        DESTINATION "${CMAKE_BINARY_DIR}/tests"
        FILES_MATCHING PATTERN "*"
)

install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/expired_runtime"
    DESTINATION "${CMAKE_BINARY_DIR}/tests"
    FILES_MATCHING PATTERN "*"
)

install(
    PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/create-pki.sh"
    DESTINATION "${CMAKE_BINARY_DIR}/tests"
)

install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/openssl-pki.conf"
    DESTINATION "${CMAKE_BINARY_DIR}/tests"
)
