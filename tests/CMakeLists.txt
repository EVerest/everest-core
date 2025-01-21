include(${CMAKE_CURRENT_SOURCE_DIR}/libocpp-unittests.cmake)

# For libocpp tests, there is one big executable, which links against the ocpp lib and all other libs.
# When it is useful to link only to the tested cpp files, a separate executable can be created for each file.
# The source files can be added to this variable, which is a list. For example:
# list(APPEND SEPARATE_UNIT_TESTS ${CMAKE_CURRENT_SOURCE_DIR}/lib/ocpp/v201/functional_blocks/test_security.cpp)
# CMake will then create a new test executable based on the filename and adds 'libocpp_' in front of it. In above
# example, a test executable 'libocpp_test_security' will be created. In this example, in the CMakeLists of
# `lib/ocpp/v201/functional_blocks`, files to link against can be added to this target / executable.
#
# For each test in this list, cmake will link agaist some 'default' cpp files (like utils and enums etc), set all
# correct flags, add a test, set definitions, etc. See below.
set(SEPARATE_UNIT_TESTS ${CMAKE_CURRENT_SOURCE_DIR}/lib/ocpp/common/utils_tests.cpp)

# Add separate tests for V201 only.
if(LIBOCPP_ENABLE_V201)
    # Add all v201 tests you don't want to include in the default test executable here.
    # Example (remove if there are real tests added):
    # list(APPEND SEPARATE_UNIT_TESTS ${CMAKE_CURRENT_SOURCE_DIR}/lib/ocpp/v201/functional_blocks/test_security.cpp)
    list(APPEND SEPARATE_UNIT_TESTS ${CMAKE_CURRENT_SOURCE_DIR}/lib/ocpp/v201/functional_blocks/test_authorization.cpp)
endif()


set(TEST_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(GCOVR_DEPENDENCIES)


add_libocpp_unittest(NAME libocpp_unit_tests PATH "")

target_link_libraries(libocpp_unit_tests PRIVATE
        ocpp
        ${GTEST_LIBRARIES}
)


# Add executables, link libraries etc for the unit tests that are separate and do not link against the ocpp lib.
# This loops over all tests added to `SEPARATE_UNIT_TESTS` and does the necessary things to make it build.
# For now, everything is added that seems to be needed in every test. If later some sources and link libraries should
# not be added here, they can always be removed from this loop and added to the test targets that actually need them.
foreach(ITEM ${SEPARATE_UNIT_TESTS})
    set(TEST_ROOT_NAME)
    cmake_path(GET ITEM STEM TEST_ROOT_NAME)
    set(TEST_NAME "libocpp_${TEST_ROOT_NAME}")
    add_libocpp_unittest(NAME ${TEST_NAME} PATH ${ITEM})
endforeach(ITEM)

# Subdirectories should be added only after adding the tests, because they have to exist for the CMakeLists.txt in the
# child directories.
add_subdirectory(lib/ocpp/common)

if(LIBOCPP_ENABLE_V16)
    add_subdirectory(lib/ocpp/v16)
endif()

if(LIBOCPP_ENABLE_V201)
    add_subdirectory(lib/ocpp/v201)
endif()

add_custom_command(TARGET libocpp_unit_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/resources/unittest_device_model.db ${CMAKE_CURRENT_BINARY_DIR}/resources/unittest_device_model.db
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/resources/unittest_device_model_missing_required.db ${CMAKE_CURRENT_BINARY_DIR}/resources/unittest_device_model_missing_required.db
        COMMAND ${CMAKE_COMMAND} -E copy ${CONFIG_FILE_LOCATION_V16} ${CONFIG_FILE_RESOURCES_LOCATION_V16}
        COMMAND ${CMAKE_COMMAND} -E copy ${USER_CONFIG_FILE_LOCATION_V16} ${USER_CONFIG_FILE_RESOURCES_LOCATION_V16}
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CONFIG_DIR_V16}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${OCPP1_6_CONFIG_DIR} ${CONFIG_DIR_V16}
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${MIGRATION_FILES_LOCATION_V16}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${MIGRATION_FILES_SOURCE_DIR_V16} ${MIGRATION_FILES_LOCATION_V16}
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${MIGRATION_FILES_LOCATION_V201}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${MIGRATION_FILES_SOURCE_DIR_V201} ${MIGRATION_FILES_LOCATION_V201}
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${MIGRATION_FILES_DEVICE_MODEL_LOCATION_V201}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${MIGRATION_FILES_DEVICE_MODEL_SOURCE_DIR_V201} ${MIGRATION_FILES_DEVICE_MODEL_LOCATION_V201}
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${DEVICE_MODEL_RESOURCES_LOCATION_V201}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEVICE_MODEL_CURRENT_RESOURCES_DIR} ${DEVICE_MODEL_RESOURCES_LOCATION_V201}
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${DEVICE_MODEL_RESOURCES_CHANGED_LOCATION_V201}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEVICE_MODEL_CURRENT_CHANGED_RESOURCES_DIR} ${DEVICE_MODEL_RESOURCES_CHANGED_LOCATION_V201}
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${DEVICE_MODEL_RESOURCES_WRONG_LOCATION_V201}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEVICE_MODEL_CURRENT_WRONG_RESOURCES_DIR} ${DEVICE_MODEL_RESOURCES_WRONG_LOCATION_V201}
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${DEVICE_MODEL_EXAMPLE_CONFIG_LOCATION_V201}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEVICE_MODEL_CURRENT_EXAMPLE_CONFIG_LOCATION_V201} ${DEVICE_MODEL_EXAMPLE_CONFIG_LOCATION_V201}
)

setup_target_for_coverage_gcovr_html(
    NAME ${PROJECT_NAME}_gcovr_coverage
    EXECUTABLE ctest
    DEPENDENCIES libocpp_unit_tests ${GCOVR_DEPENDENCIES}
    EXCLUDE "src/*" "tests/*"
)

setup_target_for_coverage_gcovr_xml(
    NAME ${PROJECT_NAME}_gcovr_coverage_xml
    EXECUTABLE ctest
    DEPENDENCIES libocpp_unit_tests ${GCOVR_DEPENDENCIES}
    EXCLUDE "src/*" "tests/*"
)
