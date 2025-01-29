set(TEST_TARGET_NAME ${PROJECT_NAME}_auth_tests)

set(TEST_SOURCES ${PROJECT_SOURCE_DIR}/modules/Auth/lib/ReservationHandler.cpp
                 ${PROJECT_SOURCE_DIR}/modules/Auth/lib/AuthHandler.cpp
                 ${PROJECT_SOURCE_DIR}/modules/Auth/lib/Connector.cpp
                 ${PROJECT_SOURCE_DIR}/modules/Auth/lib/ConnectorStateMachine.cpp)

add_executable(${TEST_TARGET_NAME} auth_tests.cpp reservation_tests.cpp ${TEST_SOURCES})

message("Current source dir: ${CMAKE_CURRENT_SOURCE_DIR}")

set(INCLUDE_DIR 
    "${CMAKE_CURRENT_SOURCE_DIR}/stubs"
    "${PROJECT_SOURCE_DIR}/modules/Auth/include"
    "${PROJECT_SOURCE_DIR}/modules/Auth/tests"
)

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
    everest::staging::helpers
    pthread
    nlohmann_json::nlohmann_json
    date::date
    date::date-tz
    )

add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})
ev_register_test_target(${TEST_TARGET_NAME})
