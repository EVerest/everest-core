set(MIGRATION_FILES_LOCATION_V16 "${CMAKE_CURRENT_BINARY_DIR}/resources/v16/migration_files")
set(MIGRATION_FILES_LOCATION_V201 "${CMAKE_CURRENT_BINARY_DIR}/resources/v201/migration_files")
set(DEVICE_MODEL_DB_LOCATION_V201 "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/everest/modules/OCPP201/device_model_storage.db")

add_executable(libocpp_unit_tests)

target_compile_definitions(libocpp_unit_tests
    PRIVATE
    MIGRATION_FILES_LOCATION_V16="${MIGRATION_FILES_LOCATION_V16}"
    MIGRATION_FILES_LOCATION_V201="${MIGRATION_FILES_LOCATION_V201}"
    MIGRATION_FILE_VERSION_V16=${MIGRATION_FILE_VERSION_V16}
    MIGRATION_FILE_VERSION_V201=${MIGRATION_FILE_VERSION_V201}
    DEVICE_MODEL_DB_LOCATION_V201="${DEVICE_MODEL_DB_LOCATION_V201}"
)

add_custom_command(TARGET libocpp_unit_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/resources/unittest_device_model.db ${CMAKE_CURRENT_BINARY_DIR}/resources/unittest_device_model.db
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/resources/unittest_device_model_missing_required.db ${CMAKE_CURRENT_BINARY_DIR}/resources/unittest_device_model_missing_required.db
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${MIGRATION_FILES_LOCATION_V16}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${MIGRATION_FILES_SOURCE_DIR_V16} ${MIGRATION_FILES_LOCATION_V16}
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${MIGRATION_FILES_LOCATION_V201}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${MIGRATION_FILES_SOURCE_DIR_V201} ${MIGRATION_FILES_LOCATION_V201}
)

add_test(libocpp_unit_tests libocpp_unit_tests)

target_include_directories(libocpp_unit_tests PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/ocpp/common
)
target_link_libraries(libocpp_unit_tests PRIVATE
        ocpp
        GTest::gmock_main
        GTest::gtest_main
)

add_subdirectory(lib/ocpp/common)

if(LIBOCPP_ENABLE_V16)
    add_subdirectory(lib/ocpp/v16)
endif()

if(LIBOCPP_ENABLE_V201)
    add_subdirectory(lib/ocpp/v201)
endif()
