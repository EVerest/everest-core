cmake_minimum_required(VERSION 3.14)

project(iee2030
    VERSION 0.1
    DESCRIPTION "Simple IEEE2030.1.1 library"
	  LANGUAGES CXX
)

find_package(everest-cmake 0.5
    PATHS ../everest-cmake
    NO_DEFAULT_PATH
)
find_package(everest-cmake 0.5)

if (NOT everest-cmake_FOUND)
    message(STATUS "Retrieving everest-cmake using FetchContent")
    include(FetchContent)
    FetchContent_Declare(
        everest-cmake
        GIT_REPOSITORY https://github.com/EVerest/everest-cmake.git
        GIT_TAG        v0.5.4
    )
    FetchContent_MakeAvailable(everest-cmake)
    set(everest-cmake_DIR "${everest-cmake_SOURCE_DIR}")
    include("${everest-cmake_SOURCE_DIR}/everest-cmake-config.cmake")
endif()

# options
option(${PROJECT_NAME}_BUILD_TESTING "Build unit tests, used if included as dependency" OFF)
option(BUILD_TESTING "Build unit tests, used if standalone project" OFF)

option(IEEE2030_INSTALL "Enable install target" ${EVC_MAIN_PROJECT})

# list of compile options
set(IEEE2030_COMPILE_OPTIONS_WARNING "-Wall;-Wextra;-Wno-unused-function;-Werror" CACHE STRING "A list of compile options used")
message(STATUS "Building libieee2030_1_1 with the following compile options: ${IEEE2030_COMPILE_OPTIONS_WARNING}")

if((${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME} OR ${PROJECT_NAME}_BUILD_TESTING) AND BUILD_TESTING)
    set(IEEE2030_BUILD_TESTING ON)
endif()

# dependencies
if (NOT DISABLE_EDM)
    evc_setup_edm()
    # In EDM mode, we can't install exports (because the dependencies usually do not install their exports)
    set(IEEE2030_INSTALL OFF)
endif()

add_subdirectory(src)

if (IEEE2030_BUILD_TESTING)
    include(CTest)
    add_subdirectory(test)
endif()

if (IEEE2030_INSTALL)
    install(
        TARGETS
            ieee2030
        EXPORT ieee2030-targets
    )

    install(
        DIRECTORY include/
        TYPE INCLUDE
        PATTERN "detail" EXCLUDE
    )

    evc_setup_package(
        NAME ieee2030
        EXPORT ieee2030-targets
        NAMESPACE ieee2030
    )
endif()
