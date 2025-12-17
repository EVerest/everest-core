set(TEST_TARGET_NAME ${PROJECT_NAME}_evse_security_tests)
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
add_compile_definitions(DEBUG_MODE_EVSE_SECURITY)

add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})

set(LIBEVSE_SECURITY_TEST_DIR "${CMAKE_BINARY_DIR}")
if (EVEREST_CORE_BUILD_TESTING)
    set(LIBEVSE_SECURITY_TEST_DIR "${CMAKE_BINARY_DIR}/lib/everest/evse_security")
endif()

file(COPY
    "${CMAKE_CURRENT_SOURCE_DIR}/generate_test_certs.sh"
    "${CMAKE_CURRENT_SOURCE_DIR}/generate_test_certs_root_multi.sh"
    "${CMAKE_CURRENT_SOURCE_DIR}/generate_test_certs_leaf_multi.sh"
    DESTINATION "${LIBEVSE_SECURITY_TEST_DIR}/tests"
)

file(COPY
    "${CMAKE_CURRENT_SOURCE_DIR}/configs"
    DESTINATION "${LIBEVSE_SECURITY_TEST_DIR}/tests"
    FILES_MATCHING PATTERN "*.cnf"
)

file(COPY
    "${CMAKE_CURRENT_SOURCE_DIR}/future_leaf"
    DESTINATION "${LIBEVSE_SECURITY_TEST_DIR}/tests"
    FILES_MATCHING PATTERN "*"
)

file(COPY
    "${CMAKE_CURRENT_SOURCE_DIR}/csms_certs"
    DESTINATION "${LIBEVSE_SECURITY_TEST_DIR}/tests"
    FILES_MATCHING PATTERN "*"
)

file(COPY
    "${CMAKE_CURRENT_SOURCE_DIR}/expired_leaf"
        DESTINATION "${LIBEVSE_SECURITY_TEST_DIR}/tests"
        FILES_MATCHING PATTERN "*"
)

file(COPY
    "${CMAKE_CURRENT_SOURCE_DIR}/expired_runtime"
    DESTINATION "${LIBEVSE_SECURITY_TEST_DIR}/tests"
    FILES_MATCHING PATTERN "*"
)

file(COPY
    "${CMAKE_CURRENT_SOURCE_DIR}/create-pki.sh"
    DESTINATION "${LIBEVSE_SECURITY_TEST_DIR}/tests"
)

file(COPY
    "${CMAKE_CURRENT_SOURCE_DIR}/openssl-pki.conf"
    DESTINATION "${LIBEVSE_SECURITY_TEST_DIR}/tests"
)
