

set(TEST_TARGET_NAME ${PROJECT_NAME}_tests)
add_executable(${TEST_TARGET_NAME})

target_include_directories(${TEST_TARGET_NAME} PUBLIC ${GTEST_INCLUDE_DIRS})

target_sources(${TEST_TARGET_NAME} PRIVATE
    tests.cpp
)

find_package(GTest REQUIRED)

target_link_libraries(${TEST_TARGET_NAME} PRIVATE
    evse_security
    ${GTEST_LIBRARIES} 
    ${GTEST_MAIN_LIBRARIES} 
)

if(LIBEVSE_CRYPTO_SUPPLIER_OPENSSL)
    add_compile_definitions(LIBEVSE_CRYPTO_SUPPLIER_OPENSSL)
endif()

add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})

install(
    PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/generate_test_certs.sh"
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
