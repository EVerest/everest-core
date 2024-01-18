add_definitions(-D_SQL_INIT_FILE="${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/everest/modules/OCPP/init.sql")
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

add_executable(libocpp_unit_tests)

add_custom_command(TARGET libocpp_unit_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/resources/unittest_device_model.db ${CMAKE_CURRENT_BINARY_DIR}/resources/unittest_device_model.db
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/resources/unittest_device_model_missing_required.db ${CMAKE_CURRENT_BINARY_DIR}/resources/unittest_device_model_missing_required.db
)

add_test(libocpp_unit_tests libocpp_unit_tests)

target_include_directories(libocpp_unit_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${GTEST_INCLUDE_DIRS})
target_link_libraries(libocpp_unit_tests PRIVATE
        ocpp
        GTest::gmock_main
        ${GTEST_LIBRARIES}
        ${GTEST_MAIN_LIBRARIES}
)
target_sources(libocpp_unit_tests PRIVATE
        comparators.cpp)

add_subdirectory(lib/ocpp/v201)
add_subdirectory(lib/ocpp/common)


add_executable(utils_tests utils_tests.cpp)

target_include_directories(utils_tests PUBLIC${GTEST_INCLUDE_DIRS})

target_link_libraries(utils_tests PRIVATE
        ocpp
        ${GTEST_LIBRARIES}
        ${GTEST_MAIN_LIBRARIES}
)

add_test(utils_tests utils_tests)
