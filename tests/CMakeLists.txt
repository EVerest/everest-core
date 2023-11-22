add_executable(database_tests database_tests.cpp)

target_include_directories(database_tests PUBLIC ${GTEST_INCLUDE_DIRS})

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

add_test(database_tests database_tests)


add_executable(ocsp_updater_tests ocsp_updater_tests.cpp comparators.cpp)

target_include_directories(ocsp_updater_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${GTEST_INCLUDE_DIRS})

find_package(GTest REQUIRED)

target_link_libraries(ocsp_updater_tests PRIVATE
        ocpp
        GTest::gmock_main
        ${GTEST_LIBRARIES}
        ${GTEST_MAIN_LIBRARIES}
)

add_test(ocsp_updater_tests ocsp_updater_tests)

