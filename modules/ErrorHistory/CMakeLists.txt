#
# AUTO GENERATED - MARKED REGIONS WILL BE KEPT
# template version 3
#

# module setup:
#   - ${MODULE_NAME}: module name
ev_setup_cpp_module()

# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1
option(BUILD_TESTING "Run unit tests" OFF)
set(CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu" ${CMAKE_PREFIX_PATH})

find_package(SQLite3 REQUIRED)
if (DISABLE_EDM)
    find_package(SQLiteCpp REQUIRED)
endif()

target_link_libraries(${MODULE_NAME}
    PRIVATE
        SQLiteCpp
        SQLite::SQLite3
)
target_sources(${MODULE_NAME}
    PRIVATE
        "ErrorDatabaseSqlite.cpp"
)
# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1

target_sources(${MODULE_NAME}
    PRIVATE
        "error_history/error_historyImpl.cpp"
)

# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
if(EVEREST_CORE_BUILD_TESTING)
    add_subdirectory(tests)
endif()
# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
