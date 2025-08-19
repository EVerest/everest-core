set(TEST_TARGET_NAME ${PROJECT_NAME}_isabellenhuetteIemDcrController_tests)

set(TEST_SOURCES ${PROJECT_SOURCE_DIR}/modules/IsabellenhuetteIemDcr/main/isabellenhuetteIemDcrController.cpp)

add_executable(${TEST_TARGET_NAME} test_isabellenhuetteIemDcrController.cpp  ${TEST_SOURCES})

add_dependencies(${TEST_TARGET_NAME} ${MODULE_NAME})

set(INCLUDE_DIR
        "main"
        "tests"
        "${PROJECT_SOURCE_DIR}/modules/IsabellenhuetteIemDcr/main"
        "${PROJECT_SOURCE_DIR}/modules/IsabellenhuetteIemDcr/tests"
)

get_target_property(GENERATED_INCLUDE_DIR generate_cpp_files EVEREST_GENERATED_INCLUDE_DIR)

target_include_directories(${TEST_TARGET_NAME} PRIVATE
        tests
        ${INCLUDE_DIR}
        ${GENERATED_INCLUDE_DIR}
)

target_link_libraries(${TEST_TARGET_NAME} PRIVATE
        GTest::gmock
        GTest::gtest_main
        everest::timer
        everest::framework
        nlohmann_json::nlohmann_json
)

add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})
ev_register_test_target(${TEST_TARGET_NAME})
