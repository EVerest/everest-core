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

setup_target_for_coverage_gcovr_html(
    NAME ${PROJECT_NAME}_gcovr_coverage
    EXECUTABLE ctest
    DEPENDENCIES ${TEST_TARGET_NAME}
    EXCLUDE "tests/*"
)

setup_target_for_coverage_gcovr_xml(
    NAME ${PROJECT_NAME}_gcovr_coverage_xml
    EXECUTABLE ctest
    DEPENDENCIES ${TEST_TARGET_NAME}
    EXCLUDE "tests/*"
)

file(COPY
    "${CMAKE_CURRENT_SOURCE_DIR}/generate_test_certs.sh"
    "${CMAKE_CURRENT_SOURCE_DIR}/generate_test_certs_multi.sh"
    DESTINATION "${CMAKE_BINARY_DIR}/tests"
)

file(COPY
    "${CMAKE_CURRENT_SOURCE_DIR}/configs"
    DESTINATION "${CMAKE_BINARY_DIR}/tests"
    FILES_MATCHING PATTERN "*.cnf"
)

file(COPY
    "${CMAKE_CURRENT_SOURCE_DIR}/future_leaf"
    DESTINATION "${CMAKE_BINARY_DIR}/tests"
    FILES_MATCHING PATTERN "*"
)

file(COPY
    "${CMAKE_CURRENT_SOURCE_DIR}/expired_leaf"
        DESTINATION "${CMAKE_BINARY_DIR}/tests"
        FILES_MATCHING PATTERN "*"
)

file(COPY
    "${CMAKE_CURRENT_SOURCE_DIR}/expired_runtime"
    DESTINATION "${CMAKE_BINARY_DIR}/tests"
    FILES_MATCHING PATTERN "*"
)

file(COPY
    "${CMAKE_CURRENT_SOURCE_DIR}/create-pki.sh"
    DESTINATION "${CMAKE_BINARY_DIR}/tests"
)

file(COPY
    "${CMAKE_CURRENT_SOURCE_DIR}/openssl-pki.conf"
    DESTINATION "${CMAKE_BINARY_DIR}/tests"
)
