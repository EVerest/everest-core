add_executable(database_tests database_tests.cpp)

target_include_directories(database_tests PUBLIC ${GTEST_INCLUDE_DIRS})

target_compile_options(database_tests PUBLIC -Wall -Wextra -Werror)

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
