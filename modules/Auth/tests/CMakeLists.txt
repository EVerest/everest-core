set(TEST_TARGET_NAME ${PROJECT_NAME}_auth_tests)
add_executable(${TEST_TARGET_NAME} auth_tests.cpp)

set(INCLUDE_DIR 
    "${PROJECT_SOURCE_DIR}/modules/Auth/include"
    "${PROJECT_SOURCE_DIR}/modules/Auth/tests")

get_target_property(GENERATED_INCLUDE_DIR generate_cpp_files EVEREST_GENERATED_INCLUDE_DIR)

target_include_directories(${TEST_TARGET_NAME} PUBLIC
    ${INCLUDE_DIR}
    ${GENERATED_INCLUDE_DIR}
)

target_link_libraries(${TEST_TARGET_NAME} PRIVATE
    GTest::gmock
    GTest::gtest_main
    everest::timer
    ${CMAKE_DL_LIBS}
    everest::log
    everest::framework
    pthread
    auth_handler
    nlohmann_json::nlohmann_json
    date::date
    date::date-tz
    )

add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})
