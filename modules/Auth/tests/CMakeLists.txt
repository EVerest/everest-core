
add_executable(auth_tests auth_tests.cpp)

set(INCLUDE_DIR 
    "${PROJECT_SOURCE_DIR}/modules/Auth/include"
    "${PROJECT_SOURCE_DIR}/modules/Auth/tests")

get_target_property(GENERATED_INCLUDE_DIR generate_cpp_files EVEREST_GENERATED_INCLUDE_DIR)

target_include_directories(auth_tests PUBLIC 
    ${INCLUDE_DIR}
    ${GENERATED_INCLUDE_DIR}
)

target_link_libraries(auth_tests PRIVATE
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

add_test(auth_tests auth_tests)
