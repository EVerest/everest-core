set(CONFIG_DIR_V16 "${CMAKE_CURRENT_BINARY_DIR}/resources/v16/")
set(MIGRATION_FILES_LOCATION_V16 "${CMAKE_CURRENT_BINARY_DIR}/resources/v16/migration_files")
set(MIGRATION_FILES_LOCATION_V201 "${CMAKE_CURRENT_BINARY_DIR}/resources/v201/migration_files")
set(CONFIG_FILE_LOCATION_V16 ${CMAKE_CURRENT_SOURCE_DIR}/config/v16/resources/config.json)
set(USER_CONFIG_FILE_LOCATION_V16 ${CMAKE_CURRENT_SOURCE_DIR}/config/v16/resources/user_config.json)
set(CONFIG_FILE_RESOURCES_LOCATION_V16 "${CMAKE_CURRENT_BINARY_DIR}/resources/config/v16/config.json")
set(USER_CONFIG_FILE_RESOURCES_LOCATION_V16 "${CMAKE_CURRENT_BINARY_DIR}/resources/config/v16/user_config.json")

set(MIGRATION_FILES_DEVICE_MODEL_LOCATION_V201 "${CMAKE_CURRENT_BINARY_DIR}/resources/v201/device_model_migration_files")
set(DEVICE_MODEL_DB_LOCATION_V201 "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/everest/modules/OCPP201/device_model_storage.db")
set(DEVICE_MODEL_RESOURCES_LOCATION_V201 "${CMAKE_CURRENT_BINARY_DIR}/resources/config/v201")
set(DEVICE_MODEL_RESOURCES_CHANGED_LOCATION_V201 "${CMAKE_CURRENT_BINARY_DIR}/resources/config/v201/changed")
set(DEVICE_MODEL_RESOURCES_WRONG_LOCATION_V201 "${CMAKE_CURRENT_BINARY_DIR}/resources/config/v201/wrong")
set(DEVICE_MODEL_CURRENT_RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/config/v201/resources)
set(DEVICE_MODEL_CURRENT_CHANGED_RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/config/v201/resources_changed)
set(DEVICE_MODEL_CURRENT_WRONG_RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/config/v201/resources_wrong)
set(DEVICE_MODEL_EXAMPLE_CONFIG_LOCATION_V201 "${CMAKE_CURRENT_BINARY_DIR}/resources/example_config/v201/component_config")
set(DEVICE_MODEL_CURRENT_EXAMPLE_CONFIG_LOCATION_V201 "${PROJECT_SOURCE_DIR}/config/v201/component_config")
set(TEST_PROFILES_LOCATION_V16 "${CMAKE_CURRENT_BINARY_DIR}/resources/profiles/v16")
set(TEST_PROFILES_LOCATION_V201 "${CMAKE_CURRENT_BINARY_DIR}/resources/profiles/v201")

# Add variables that can be used for all tests if needed.
set(GTEST_LIBRARIES GTest::gmock_main GTest::gtest_main)
set(TEST_COMPILE_OPTIONS -pedantic-errors)
set(TEST_COMPILE_DEFINITIONS
    CONFIG_FILE_LOCATION_V16="${CONFIG_FILE_RESOURCES_LOCATION_V16}"
    USER_CONFIG_FILE_LOCATION_V16="${USER_CONFIG_FILE_RESOURCES_LOCATION_V16}"
    CONFIG_DIR_V16="${CONFIG_DIR_V16}"
    MIGRATION_FILES_LOCATION_V16="${MIGRATION_FILES_LOCATION_V16}"
    MIGRATION_FILES_LOCATION_V201="${MIGRATION_FILES_LOCATION_V201}"
    MIGRATION_FILE_VERSION_V16=${MIGRATION_FILE_VERSION_V16}
    MIGRATION_FILE_VERSION_V201=${MIGRATION_FILE_VERSION_V201}
    DEVICE_MODEL_DB_LOCATION_V201="${DEVICE_MODEL_DB_LOCATION_V201}"
    TEST_PROFILES_LOCATION_V16="${TEST_PROFILES_LOCATION_V16}"
    TEST_PROFILES_LOCATION_V201="${TEST_PROFILES_LOCATION_V201}")
set(TEST_COMPILE_FEATURES cxx_std_17)
set(LIBOCPP_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/include)
set(LIBOCPP_LIB_PATH ${PROJECT_SOURCE_DIR}/lib)
set(LIBOCPP_3RDPARTY_PATH ${PROJECT_SOURCE_DIR}/3rd_party)
set(TEST_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}
                             ${CMAKE_CURRENT_SOURCE_DIR}/lib/ocpp/common
                             ${LIBOCPP_INCLUDE_PATH}
)
# If the test is not linked against the ocpp library, most probably those libraries are needed to link against.
set(LIBOCPP_TEST_DEFAULT_LINK_LIBRARIES
        SQLite::SQLite3
        nlohmann_json::nlohmann_json
        date::date-tz
        everest::log
        everest::evse_security
)

# If the test is not linked against the ocpp library, those default sources can be linked against, they will often
# be needed to link against.
set(LIBOCPP_TEST_INCLUDE_COMMON_SOURCES ${LIBOCPP_LIB_PATH}/ocpp/common/types.cpp
                                        ${LIBOCPP_LIB_PATH}/ocpp/common/ocpp_logging.cpp
                                        ${LIBOCPP_LIB_PATH}/ocpp/common/utils.cpp
                                        ${LIBOCPP_LIB_PATH}/ocpp/common/call_types.cpp
                                        ${LIBOCPP_LIB_PATH}/ocpp/common/evse_security.cpp
                                        ${LIBOCPP_LIB_PATH}/ocpp/common/database/database_connection.cpp
                                        ${LIBOCPP_LIB_PATH}/ocpp/common/database/database_handler_common.cpp
                                        ${LIBOCPP_LIB_PATH}/ocpp/common/database/sqlite_statement.cpp
                                        ${LIBOCPP_LIB_PATH}/ocpp/common/database/database_schema_updater.cpp
)

# If the test is not linked against the ocpp library, those default sources for ocpp v2.0.1 can be linked against, they
# will often be needed to link against.
set(LIBOCPP_TEST_INCLUDE_V201_SOURCES ${LIBOCPP_LIB_PATH}/ocpp/v16/ocpp_enums.cpp   # This is currently still needed but might be removed in the future.
                                      ${LIBOCPP_LIB_PATH}/ocpp/v201/ctrlr_component_variables.cpp
                                      ${LIBOCPP_LIB_PATH}/ocpp/v201/types.cpp
                                      ${LIBOCPP_LIB_PATH}/ocpp/v201/ocpp_types.cpp
                                      ${LIBOCPP_LIB_PATH}/ocpp/v201/enums.cpp
                                      ${LIBOCPP_LIB_PATH}/ocpp/v201/ocpp_enums.cpp
                                      ${LIBOCPP_LIB_PATH}/ocpp/v201/device_model.cpp
                                      ${LIBOCPP_LIB_PATH}/ocpp/v201/init_device_model_db.cpp
                                      ${LIBOCPP_LIB_PATH}/ocpp/v201/device_model_storage_sqlite.cpp
                                      ${LIBOCPP_LIB_PATH}/ocpp/v201/utils.cpp
)

function(add_libocpp_unittest)
    set(oneValueArgs NAME PATH)
    set(options)
    set(multiValueArgs)
    cmake_parse_arguments(arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    if (arg_PATH)
        add_executable(${arg_NAME} ${arg_PATH})
    else ()
        add_executable(${arg_NAME})
    endif()
    add_test(NAME ${arg_NAME} COMMAND ${arg_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    list(APPEND GCOVR_DEPENDENCIES ${arg_NAME})
    target_link_libraries(${arg_NAME} PUBLIC ${GTEST_LIBRARIES})
    target_link_libraries(${arg_NAME} PRIVATE ${LIBOCPP_TEST_DEFAULT_LINK_LIBRARIES})
    target_compile_definitions(${arg_NAME}
        PRIVATE
            ${TEST_COMPILE_DEFINITIONS}
            MIGRATION_FILE_VERSION_V16=${MIGRATION_FILE_VERSION_V16}
            MIGRATION_FILE_VERSION_V201=${MIGRATION_FILE_VERSION_V201}
            MIGRATION_DEVICE_MODEL_FILE_VERSION_V201=${MIGRATION_DEVICE_MODEL_FILE_VERSION_V201})
    target_compile_options(${arg_NAME} PRIVATE ${TEST_COMPILE_OPTIONS})
    target_compile_features(${arg_NAME} PUBLIC ${TEST_COMPILE_FEATURES})
    target_include_directories(${arg_NAME} PRIVATE ${TEST_INCLUDE_DIRECTORIES})
    message("Add test ${arg_NAME}")
endfunction()
