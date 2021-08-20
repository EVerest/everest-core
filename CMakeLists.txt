cmake_minimum_required(VERSION 3.14.7)

project(timer VERSION 0.1
            DESCRIPTION "EVerest timer library"
            LANGUAGES CXX C)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(BUILD_EXAMPLES "Build examples" ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

option(EVEREST_CROSS_COMPILE "Cross compile the EVerest timer library" OFF)
if(EVEREST_CROSS_COMPILE)
    if(NOT BOOST_ROOT)
        message(FATAL_ERROR "Please set BOOST_ROOT to a cross compiled version of Boost!")
    endif()
    set(Boost_USE_STATIC_LIBS ON)
    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_SYSTEM_PROCESSOR arm)
    set(BOOST_LOG_DYN_LINK OFF)
    set(BOOST_ALL_DYN_LINK OFF)
else()
    add_definitions(-DBOOST_LOG_DYN_LINK)
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/dist" CACHE PATH "..." FORCE)
endif()

option(BUILD_TESTING "Run unit tests" OFF)

# configure clang-tidy if requested
option(CMAKE_RUN_CLANG_TIDY "Run clang-tidy" OFF)
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
    doxygen_add_docs(${PROJECT_NAME}doxygen include lib src)
else()
    message("Doxygen is needed to generate documentation")
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")


find_package(Boost COMPONENTS filesystem program_options system thread REQUIRED)

# testing
if(BUILD_TESTING)
    include(CTest)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)

    include(CodeCoverage)

    append_coverage_compiler_flags()

    add_subdirectory(tests)

    setup_target_for_coverage_gcovr_html(
        NAME gcovr_coverage
        EXECUTABLE test_config
        DEPENDENCIES test_config everest
    )

    setup_target_for_coverage_lcov(
        NAME lcov_coverage
        EXECUTABLE test_config
        DEPENDENCIES test_config everest
    )
else()
    message("Not running unit tests")
endif()

find_package(EDM REQUIRED)

# logging library
add_subdirectory(lib)

if(BUILD_EXAMPLES)
    # example code
    add_subdirectory(examples)
endif()
