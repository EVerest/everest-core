cmake_minimum_required(VERSION 3.14.7)

project(everest-core
    VERSION 2024.3.0
    DESCRIPTION "The open operating system for e-mobility charging stations"
	LANGUAGES CXX C
)

find_package(everest-cmake 0.1
    COMPONENTS bundling
    PATHS ../everest-cmake
)

if (NOT everest-cmake_FOUND)
    message(STATUS "Retreiving everest-cmake using FetchContent")
    include(FetchContent)
    FetchContent_Declare(
        everest-cmake
        GIT_REPOSITORY https://github.com/EVerest/everest-cmake.git
        GIT_TAG        main
    )
    FetchContent_MakeAvailable(everest-cmake)
    set(everest-cmake_DIR "${everest-cmake_SOURCE_DIR}")
    set(everest-cmake_FIND_COMPONENTS "bundling")
    include("${everest-cmake_SOURCE_DIR}/everest-cmake-config.cmake")
endif()

# make own cmake modules available
list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# test whether linking with libatomic is required on some platforms
include(CheckAtomic)
if(HAVE_CXX_ATOMICS_WITH_LIB OR HAVE_CXX_ATOMICS64_WITH_LIB)
    set(ATOMIC_LIBS "atomic")
else()
    set(ATOMIC_LIBS "")
endif()

option(CREATE_SYMLINKS "Create symlinks to javascript modules and auxillary files - for development purposes" OFF)
option(CMAKE_RUN_CLANG_TIDY "Run clang-tidy" OFF)
option(ISO15118_2_GENERATE_AND_INSTALL_CERTIFICATES "Automatically generate and install certificates for development purposes" ON)
option(EVEREST_ENABLE_RUN_SCRIPT_GENERATION "Enables the generation of run scripts (convenience scripts for starting available configurations)" ON)
option(${PROJECT_NAME}_BUILD_TESTING "Build unit tests, used if included as dependency" OFF)
option(BUILD_TESTING "Build unit tests, used if standalone project" OFF)
if((${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME} OR ${PROJECT_NAME}_BUILD_TESTING) AND BUILD_TESTING)
    set(EVEREST_CORE_BUILD_TESTING ON)
endif()
# This is a flag for building development tests, but not necessarily to run them, for expample in case
# tests requires hardware.
option(BUILD_DEV_TESTS "Build dev tests" OFF)

# Already include CTest here to allow it to find tests defined in subdirectories like lib and modules
if(EVEREST_CORE_BUILD_TESTING)
    include(CTest)

    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)

    evc_include(CodeCoverage)

    append_coverage_compiler_flags()
endif()

include(ev-define-dependency)

include("module-dependencies.cmake")

find_package(Boost
    COMPONENTS
        filesystem
        program_options
        system
        thread
    REQUIRED
)

if(NOT DISABLE_EDM)
    # FIXME (aw): this implicit definition for child projects is hacky
    set(THIRD_PARTY_APP_DST "${CMAKE_INSTALL_LIBEXECDIR}/everest/3rd_party")

    evc_setup_edm()

    # FIXME (aw): we need to set this by hand due to edm
    set(EVEREST_SCHEMA_DIR ${everest-framework_SOURCE_DIR}/schemas)
else()
    find_package(everest-framework REQUIRED)
    find_package(everest-modbus REQUIRED)
    find_package(everest-ocpp REQUIRED)
    find_package(everest-openv2g REQUIRED)

    find_package(PalSigslot REQUIRED)

    find_package(fsm REQUIRED)
    find_package(slac REQUIRED)

    find_package(pugixml REQUIRED)
    find_package(CURL 7.84.0 REQUIRED)
    find_package(ryml REQUIRED)
endif()

add_subdirectory(lib)

include(ev-project-bootstrap)

ev_add_project()

# create MF_ROOT_CA if not available
if (ISO15118_2_GENERATE_AND_INSTALL_CERTIFICATES)
    file(TOUCH config/certs/ca/mf/MF_ROOT_CA.pem)
endif()

# config
# FIXME (aw): this should be optional
add_subdirectory(config)

# install docker related files
install(
    DIRECTORY "cmake/assets/docker"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/everest"
)

ev_install_project()

# configure clang-tidy if requested
if(CMAKE_RUN_CLANG_TIDY)
    message("Enabling clang-tidy")
    set(CMAKE_CXX_CLANG_TIDY clang-tidy)
endif()

# build doxygen documentation if doxygen is available
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set( DOXYGEN_OUTPUT_DIRECTORY dist/docs )
    doxygen_add_docs(doxygen-${PROJECT_NAME} everest.js include lib src)
endif()

# testing
if(EVEREST_CORE_BUILD_TESTING)
    add_subdirectory(tests)

    setup_target_for_coverage_gcovr_html(
        NAME ${PROJECT_NAME}_gcovr_coverage
        EXECUTABLE test_config
        DEPENDENCIES test_config everest
    )

    setup_target_for_coverage_lcov(
        NAME ${PROJECT_NAME}_lcov_coverage
        EXECUTABLE test_config
        DEPENDENCIES test_config everest
    )
else()
    message("Not running unit tests")
endif()

# convenience target for integration tests
add_custom_target(install_everest_testing
    COMMAND
        if [ -z "${CPM_PACKAGE_everest-utils_SOURCE_DIR}" ] \;
        then echo "Could not determine location of everest-utils, please install everest-testing manually!" \;
        else echo "Found everest-utils at ${CPM_PACKAGE_everest-utils_SOURCE_DIR}" \;
             ${PYTHON_EXECUTABLE} -m pip install "${CPM_PACKAGE_everest-utils_SOURCE_DIR}/everest-testing" \;
        fi\;
    DEPENDS
        everestpy_pip_install_dist
    COMMENT
        "Installing dependencies for testing EVerest"
)
