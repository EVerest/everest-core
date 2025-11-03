cmake_minimum_required(VERSION 3.14)

project(everest-sqlite VERSION 0.1.3
        DESCRIPTION "SQLite wrapper for EVerest"
        LANGUAGES CXX C
)

find_package(everest-cmake 0.5 REQUIRED
    PATHS ../everest-cmake
)

option(${PROJECT_NAME}_BUILD_TESTING "Build unit tests, used if included as dependency" OFF)
option(BUILD_TESTING "Build unit tests, used if standalone project" OFF)
option(EVEREST_SQLITE_INSTALL "Install the library (shared data might be installed anyway)" ${EVC_MAIN_PROJECT})

if((${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME} OR ${PROJECT_NAME}_BUILD_TESTING) AND BUILD_TESTING)
    set(EVEREST_SQLITE_BUILD_TESTING ON)
    # this policy allows us to link gcov to targets defined in other directories
    if(POLICY CMP0079)
        set(CMAKE_POLICY_DEFAULT_CMP0079 NEW)
    endif()
endif()

find_package(SQLite3 REQUIRED)

if (NOT DISABLE_EDM)
    evc_setup_edm()

    # In EDM mode, we can't install exports (because the dependencies usually do not install their exports)
    set(EVEREST_SQLITE_INSTALL OFF)
else()
    find_package(nlohmann_json REQUIRED)
    find_package(everest-log REQUIRED)
    if (EVEREST_SQLITE_BUILD_TESTING)
        include(FetchContent)
        FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/release-1.12.1.zip
        )
        FetchContent_MakeAvailable(googletest)
    endif()
endif()


# options
option(${PROJECT_NAME}_BUILD_TESTING "Build unit tests, used if included as dependency" OFF)
option(BUILD_TESTING "Build unit tests, used if standalone project" OFF)

add_subdirectory(lib)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# packaging
if (EVEREST_SQLITE_INSTALL)
    set_target_properties(everest_sqlite PROPERTIES EXPORT_NAME sqlite)

    install(
        TARGETS everest_sqlite
        EXPORT everest_sqlite-targets
        LIBRARY
    )

    install(
        DIRECTORY include/
        TYPE INCLUDE
    )

    evc_setup_package(
        NAME everest-sqlite
        NAMESPACE everest
        EXPORT everest_sqlite-targets
        ADDITIONAL_CONTENT
            "find_dependency(everest-log)"
            "find_dependency(SQLite3)"
            "include($\{CMAKE_CURRENT_LIST_DIR\}/cmake/CollectMigrationFiles.cmake)"
    )
endif()

if(EVEREST_SQLITE_BUILD_TESTING)
    include(CTest)
    add_subdirectory(tests)
    set(CMAKE_BUILD_TYPE Debug)
endif()
