add_executable(module_error_history_tests)

target_sources(module_error_history_tests
    PRIVATE
        error_database_sqlite_tests.cpp
        ../ErrorDatabaseSqlite.cpp
        helpers.cpp
)

# target_include_directories(module_error_history_tests
#     PRIVATE
# )

target_link_libraries(module_error_history_tests
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
    catch_discover_tests(module_error_history_tests)
endif()

add_test(module_error_history_tests module_error_history_tests)
