cmake_minimum_required(VERSION 3.16)

project(everest-log
    VERSION 0.2.4
    DESCRIPTION "EVerest logging library"
    LANGUAGES CXX C
)

find_package(everest-cmake 0.5 REQUIRED
    PATHS ../everest-cmake
)

# options
option(BUILD_BACKTRACE_SUPPORT "Build with backtrace support from libbacktrace" OFF)
option(${PROJECT_NAME}_BUILD_TESTING "Build unit tests, used if included as dependency" OFF)
option(BUILD_TESTING "Run unit tests" OFF)
option(BUILD_EXAMPLES "Build liblog example binaries." OFF)
option(LOG_INSTALL "Install the library (shared data might be installed anyway)" ${EVC_MAIN_PROJECT})
option(CMAKE_RUN_CLANG_TIDY "Run clang-tidy" OFF)
option(LIBLOG_USE_BOOST_FILESYSTEM "Usage of boost/filesystem.hpp instead of std::filesystem" OFF)

if((${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME} OR ${PROJECT_NAME}_BUILD_TESTING) AND BUILD_TESTING)
    set(EVEREST_LIBLOG_BUILD_TESTING ON)
    # this policy allows us to link gcov to targets defined in other directories
    if(POLICY CMP0079)
        set(CMAKE_POLICY_DEFAULT_CMP0079 NEW)
    endif()
endif()

if (NOT DISABLE_EDM)
    evc_setup_edm()

    # In EDM mode, we can't install exports (because the dependencies usually do not install their exports)
    set(LOG_INSTALL OFF)
else()
    if (EVEREST_LIBLOG_BUILD_TESTING)
        include(FetchContent)
        FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/release-1.12.1.zip
        )
        FetchContent_MakeAvailable(googletest)
    endif()
endif()

# library dependencies

if (LIBLOG_USE_BOOST_FILESYSTEM) 
    message(STATUS "Using boost/filesystem instead of std::filesystem")
    find_package(Boost COMPONENTS log_setup log filesystem REQUIRED)
else()
    find_package(Boost COMPONENTS log_setup log REQUIRED)
endif()


# third party dependencies
add_subdirectory(3rd_party)

# logging library
add_subdirectory(lib)

# packaging
install(
    FILES examples/logging.ini
    DESTINATION ${CMAKE_INSTALL_DATADIR}/everest/log
    RENAME example-config.ini
)

if (LOG_INSTALL)
    set_target_properties(everest_log PROPERTIES EXPORT_NAME log)

    install(
        TARGETS everest_log
        EXPORT log-targets
    )

    install(
        DIRECTORY include/
        TYPE INCLUDE
    )

    if (BUILD_BACKTRACE_SUPPORT)
    #   FIXME (aw): if statically build, we would need to install libbacktrace too
    endif()

    evc_setup_package(
        NAME everest-log
        NAMESPACE everest
        EXPORT log-targets
        ADDITIONAL_CONTENT
            "find_dependency(Boost COMPONENTS log_setup log)"
    )
endif()


# testing
if(EVEREST_LIBLOG_BUILD_TESTING)
    include(CTest)
    add_subdirectory(tests)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(BUILD_EXAMPLES)
    message("Building liblog example binaries.")
    add_subdirectory(examples)
else()
    message("Not building liblog example binaries.")
endif()

# configure clang-tidy if requested
if(CMAKE_RUN_CLANG_TIDY)
    message("Running clang-tidy")
    string(CONCAT CLANG_TIDY_CHECKS "*,"
                                    "-llvmlibc*,"
                                    "-fuchsia-default-arguments-calls,"
                                    "-fuchsia-overloaded-operator,"
                                    "-fuchsia-statically-constructed-objects,"
                                    "-readability-function-cognitive-complexity,"
                                    "-modernize-use-trailing-return-type,"
                                    "-abseil-string-find-startswith,"
                                    "-abseil-string-find-str-contains,"
                                    ";")
    set(CMAKE_CXX_CLANG_TIDY 
    clang-tidy;
    -header-filter='.*'
    -checks=${CLANG_TIDY_CHECKS})
endif()

# build doxygen documentation if doxygen is available
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set( DOXYGEN_OUTPUT_DIRECTORY dist/docs )
    doxygen_add_docs(doxygen-${PROJECT_NAME} include lib src)
else()
    message("Doxygen is needed to generate documentation")
endif()
