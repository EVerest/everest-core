set(TEST_TARGET_NAME ${PROJECT_NAME}_API_serialize_tests)
add_executable(${TEST_TARGET_NAME})

set(INCLUDE_DIR
  "${PROJECT_SOURCE_DIR}/lib/everest/everest_api_types/include"
  "${PROJECT_SOURCE_DIR}/lib/everest/everest_api_types/tests"
)

target_include_directories(${TEST_TARGET_NAME} PUBLIC
  ${INCLUDE_DIR}
)

target_sources(${TEST_TARGET_NAME} PRIVATE
  manual_tests/serialization/generic.hpp
  manual_tests/serialization/generic.cpp
)

target_link_libraries(${TEST_TARGET_NAME} PRIVATE
  GTest::gmock
  GTest::gtest_main
  everest::log
  everest::framework
  everest::everest_api_types
)

add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})

file(GLOB API_FILES ${PROJECT_SOURCE_DIR}/lib/everest/everest_api_types/include/everest_api_types/*/API.hpp)

set(GENERATED_TESTS_LOCATION ${CMAKE_BINARY_DIR}/generated/lib/everest/everest_api_types/tests/)

#update tests on api file change

set(
    GENERATE_COMMAND
    ${Python3_EXECUTABLE} -B "${PROJECT_SOURCE_DIR}/lib/everest/everest_api_types/tests/test_file_autogenerator/pythonProject/main.py"
     "${PROJECT_SOURCE_DIR}/lib/everest/everest_api_types/include/"
     "${PROJECT_SOURCE_DIR}/lib/everest/everest_api_types/tests/"
     "${PROJECT_SOURCE_DIR}/lib/everest/everest_api_types/tests/test_file_autogenerator/pythonProject/disable.csv"
     "${GENERATED_TESTS_LOCATION}"
    VERBATIM
)

ADD_CUSTOM_COMMAND(OUTPUT ${GENERATED_TESTS_LOCATION}
    COMMAND
    ${GENERATE_COMMAND}
    DEPENDS ${API_FILES}
)

ADD_CUSTOM_TARGET(Generate_new_tests_on_file_change_only ALL   DEPENDS ${GENERATED_TESTS_LOCATION})

add_dependencies(${TEST_TARGET_NAME} Generate_new_tests_on_file_change_only)

#generate tests for the first time

if(NOT EXISTS ${GENERATED_TESTS_LOCATION}) 
    EXECUTE_PROCESS(COMMAND ${GENERATE_COMMAND})
endif()


add_subdirectory(${GENERATED_TESTS_LOCATION} ${GENERATED_TESTS_LOCATION})
