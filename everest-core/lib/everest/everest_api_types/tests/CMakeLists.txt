set(TEST_TARGET_NAME ${PROJECT_NAME}_API_serialize_tests)

add_executable(${TEST_TARGET_NAME})

set(EXPECTED_TYPE_HASHES_CSV "${CMAKE_CURRENT_SOURCE_DIR}/expected_types_file_hashes.csv")
set(TYPES_FILES_DIR "${PROJECT_SOURCE_DIR}/types")
set(ACTUAL_TYPE_HASHES_CSV ${CMAKE_BINARY_DIR}/generated/lib/everest/everest_api_types/tests/actual_types_file_hashes.csv)
file(GLOB ALL_TYPE_FILES "${TYPES_FILES_DIR}/*")

set(GENERATE_ACTUAL_HASHES_CMD ${CMAKE_CURRENT_SOURCE_DIR}/manual_tests/source_file_hash_check/create_file_hashes.cmake)
set(VERIFY_HASHES_CMD ${CMAKE_CURRENT_SOURCE_DIR}/manual_tests/source_file_hash_check/verify_file_hashes.cmake)

add_custom_command(
  OUTPUT ${ACTUAL_TYPE_HASHES_CSV}
  COMMAND ${CMAKE_COMMAND}
        -DEXPECTED_CSV=${EXPECTED_TYPE_HASHES_CSV}
        -DPROJECT_SRC_FOLDER=${PROJECT_SOURCE_DIR}
        -DACTUAL_CSV=${ACTUAL_TYPE_HASHES_CSV}
        -P ${GENERATE_ACTUAL_HASHES_CMD}
  # On mismatch: Print a warning during configuration/build time
  COMMAND ${CMAKE_COMMAND}
        -DEXPECTED_CSV=${EXPECTED_TYPE_HASHES_CSV}
        -DACTUAL_CSV=${ACTUAL_TYPE_HASHES_CSV}
        -P ${VERIFY_HASHES_CMD}
  DEPENDS ${GENERATE_ACTUAL_HASHES_CMD} ${VERIFY_HASHES_CMD} ${EXPECTED_TYPE_HASHES_CSV} ${ALL_TYPE_FILES}
  COMMENT "Generating ${ACTUAL_TYPE_HASHES_CSV} from source files..."
)

add_custom_target(generate_actual_hash_csv ALL
    DEPENDS ${ACTUAL_TYPE_HASHES_CSV}
)

target_compile_definitions(${TEST_TARGET_NAME} PRIVATE
  EXPECTED_TYPE_FILE_HASHES_CSV_PATH="${EXPECTED_TYPE_HASHES_CSV}"
  ACTUAL_TYPE_FILE_HASHES_CSV_PATH="${ACTUAL_TYPE_HASHES_CSV}"
)

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
  manual_tests/source_file_hash_check/source_file_hash_check.cpp
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
    ${Python3_EXECUTABLE} -B "${PROJECT_SOURCE_DIR}/lib/everest/everest_api_types/tests/test_file_autogenerator/main.py"
     "${PROJECT_SOURCE_DIR}/lib/everest/everest_api_types/include/"
     "${PROJECT_SOURCE_DIR}/lib/everest/everest_api_types/tests/"
     "${PROJECT_SOURCE_DIR}/lib/everest/everest_api_types/tests/test_file_autogenerator/disable.csv"
     "${GENERATED_TESTS_LOCATION}"
    VERBATIM
)

add_custom_command(OUTPUT ${GENERATED_TESTS_LOCATION}
    COMMAND
    ${GENERATE_COMMAND}
    DEPENDS ${API_FILES}
)

add_custom_target(generate_new_tests_on_file_change_only ALL   DEPENDS ${GENERATED_TESTS_LOCATION})

add_dependencies(${TEST_TARGET_NAME} generate_new_tests_on_file_change_only generate_actual_hash_csv)

#generate tests for the first time

if(NOT EXISTS ${GENERATED_TESTS_LOCATION}) 
    execute_process(COMMAND ${GENERATE_COMMAND})
endif()


add_subdirectory(${GENERATED_TESTS_LOCATION} ${GENERATED_TESTS_LOCATION})
