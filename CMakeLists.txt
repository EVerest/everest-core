cmake_minimum_required(VERSION 3.14)

project(iso15118
    VERSION 0.8.0
    DESCRIPTION "iso15118 library suite"
	LANGUAGES CXX C
)

find_package(everest-cmake 0.5
    PATHS ../everest-cmake
    NO_DEFAULT_PATH
)
find_package(everest-cmake 0.5)

find_package(OpenSSL 3 REQUIRED)

if (NOT everest-cmake_FOUND)
    message(STATUS "Retrieving everest-cmake using FetchContent")
    include(FetchContent)
    FetchContent_Declare(
        everest-cmake
        GIT_REPOSITORY https://github.com/EVerest/everest-cmake.git
        GIT_TAG        v0.5.3
    )
    FetchContent_MakeAvailable(everest-cmake)
    set(everest-cmake_DIR "${everest-cmake_SOURCE_DIR}")
    include("${everest-cmake_SOURCE_DIR}/everest-cmake-config.cmake")
endif()

# options
option(${PROJECT_NAME}_BUILD_TESTING "Build unit tests, used if included as dependency" OFF)
option(BUILD_TESTING "Build unit tests, used if standalone project" OFF)
option(OPT_AUTODOWNLOAD_ISO20_SCHEMAS "\
Automatically download ISO15118-20 schemas.  Note: by setting this option to \
true and hence downloading the schema files, YOU accept the ISO Customer \
Licence Agreement (“Licence Agreement”), clauses 1. ISO’s Copyright, \
7. Termination, 8. Limitations, and 9. Governing Law." OFF)

option(ISO15118_INSTALL "Enable install target" ${EVC_MAIN_PROJECT})

# list of compile options
set(ISO15118_COMPILE_OPTIONS_WARNING "-Wall;-Wextra;-Wno-unused-function;-Werror" CACHE STRING "A list of compile options used")
message(STATUS "Building libiso15118 with the following compile options: ${ISO15118_COMPILE_OPTIONS_WARNING}")

if((${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME} OR ${PROJECT_NAME}_BUILD_TESTING) AND BUILD_TESTING)
    set(ISO15118_BUILD_TESTING ON)
endif()

# dependencies
if (NOT DISABLE_EDM)
    evc_setup_edm()
    # In EDM mode, we can't install exports (because the dependencies usually do not install their exports)
    set(ISO15118_INSTALL OFF)
else()
    find_package(cbv2g REQUIRED)
endif()

add_subdirectory(input)
add_subdirectory(src)

if (ISO15118_BUILD_TESTING)
    include(CTest)
    add_subdirectory(test)
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
