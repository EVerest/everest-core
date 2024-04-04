set(TARGET_NAME ${PROJECT_NAME}_module_error_history_tests)
add_executable(${TARGET_NAME})

target_sources(${TARGET_NAME}
    PRIVATE
        error_database_sqlite_tests.cpp
        ../ErrorDatabaseSqlite.cpp
        helpers.cpp
)

# target_include_directories(module_error_history_tests
#     PRIVATE
# )

target_link_libraries(${TARGET_NAME}
    PRIVATE
        everest::framework
        everest::log
        SQLiteCpp
        SQLite::SQLite3
        Catch2::Catch2WithMain
)
if(NOT DISABLE_EDM)
    list(APPEND CMAKE_MODULE_PATH ${CPM_PACKAGE_catch2_SOURCE_DIR}/extras)
    include(Catch)
    catch_discover_tests(${TARGET_NAME})
endif()

add_test(${TARGET_NAME} ${TARGET_NAME})
