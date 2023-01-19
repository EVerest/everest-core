add_executable(database_tests database_tests.cpp)
add_executable(pki_handler_tests pki_handler_tests.cpp)

target_include_directories(database_tests PUBLIC ${GTEST_INCLUDE_DIRS})
target_include_directories(pki_handler_tests PUBLIC ${GTEST_INCLUDE_DIRS})

find_package(GTest REQUIRED)

target_link_libraries(database_tests PRIVATE
    ocpp
    date::date
    nlohmann_json::nlohmann_json
    nlohmann_json_schema_validator
    ${GTEST_LIBRARIES} 
    ${GTEST_MAIN_LIBRARIES} 
    pthread
    )

    target_link_libraries(pki_handler_tests PRIVATE
    ocpp
    nlohmann_json::nlohmann_json
    nlohmann_json_schema_validator
    ${GTEST_LIBRARIES} 
    ${GTEST_MAIN_LIBRARIES} 
    pthread
    )

add_test(database_tests database_tests)
add_test(pki_handler_tests pki_handler_tests)

install(
    PROGRAMS "generate_test_certs.sh"
    DESTINATION "${CMAKE_BINARY_DIR}/tests"
)

install(
    DIRECTORY "configs"
    DESTINATION "${CMAKE_BINARY_DIR}/tests"
    FILES_MATCHING PATTERN "*.cnf"
)
