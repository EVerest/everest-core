cmake_minimum_required(VERSION 3.14)

project(iso15118
    VERSION 0.1.1
    DESCRIPTION "iso15118 library suite"
	LANGUAGES CXX C
)

find_package(everest-cmake 0.1 REQUIRED
    PATHS ../everest-cmake
)

# options
option(${PROJECT_NAME}_BUILD_TESTING "Build unit tests, used if included as dependency" OFF)
option(BUILD_TESTING "Build unit tests, used if standalone project" OFF)
option(OPT_AUTODOWNLOAD_ISO20_SCHEMAS "\
Automatically download ISO15118-20 schemas.  Note: by setting this option to \
true and hence downloading the schema files, YOU accept the ISO Customer \
Licence Agreement (“Licence Agreement”), clauses 1. ISO’s Copyright, \
7. Termination, 8. Limitations, and 9. Governing Law." OFF)

option(ISO15118_INSTALL "Enable install target" ${EVC_MAIN_PROJECT})

option(ISO15118_LINK_CUSTOM_MBEDTLS "\
Link against a pinned mbedtls library\
" ON)

option(USE_OPENSSL "Use OpenSSL" ON)

if (USE_OPENSSL)
    find_package(OpenSSL 3 REQUIRED)
endif()

# list of compile options
set(ISO15118_COMPILE_OPTIONS_WARNING "-Wall;-Wextra;-Wno-unused-function;-Werror" CACHE STRING "A list of compile options used")
message(STATUS "Building libiso15118 with the following compile options: ${ISO15118_COMPILE_OPTIONS_WARNING}")

if((${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME} OR ${PROJECT_NAME}_BUILD_TESTING) AND BUILD_TESTING)
    set(ISO15118_BUILD_TESTING ON)
    evc_include(CodeCoverage)
    append_coverage_compiler_flags()
endif()

# dependencies
if (NOT DISABLE_EDM)
    evc_setup_edm()
    # In EDM mode, we can't install exports (because the dependencies usually do not install their exports)
    set(ISO15118_INSTALL OFF)
else()
    find_package(fsm REQUIRED)
    find_package(cbv2g REQUIRED)
endif()

add_subdirectory(3rd_party)
add_subdirectory(input)
add_subdirectory(src)

if (ISO15118_BUILD_TESTING)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE) # TODO(sl): Check if necessary

    include(CTest)
    list(APPEND CMAKE_MODULE_PATH ${CPM_PACKAGE_catch2_SOURCE_DIR}/extras)
    add_subdirectory(test)

    # FIXME(sl): Set gcovr correctly so that it finds the catch2 files. 
    set(GCOVR_ADDITIONAL_ARGS "--gcov-ignore-errors=no_working_dir_found")
    setup_target_for_coverage_gcovr_html(
        NAME ${PROJECT_NAME}_gcovr_coverage
        EXECUTABLE ctest
        EXCLUDE "test/*" "build/*"
    )

    setup_target_for_coverage_gcovr_xml(
        NAME ${PROJECT_NAME}_gcovr_coverage_xml
        EXECUTABLE ctest
        EXCLUDE "test/*" "build/*"
    )
endif()

if (ISO15118_INSTALL)
    install(
        TARGETS
            iso15118
        EXPORT iso15118-targets
    )

    install(
        DIRECTORY include/
        TYPE INCLUDE
        PATTERN "detail" EXCLUDE
    )

    evc_setup_package(
        NAME iso15118
        EXPORT iso15118-targets
        NAMESPACE iso15118
    )
endif()
