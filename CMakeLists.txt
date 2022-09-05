cmake_minimum_required(VERSION 3.14.7)

project(everest-core
    VERSION 0.1
    DESCRIPTION "The open operating system for e-mobility charging stations"
	LANGUAGES CXX C
)

find_package(everest-cmake 0.10 REQUIRED
    COMPONENTS bundling
    PATHS ../everest-cmake
)

# make own cmake modules available
list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(ev-cli)

option(CREATE_SYMLINKS "Create symlinks to javascript modules and auxillary files - for development purposes" OFF)
option(CMAKE_RUN_CLANG_TIDY "Run clang-tidy" OFF)
option(BUILD_TESTING "Run unit tests" OFF)

# dependencies
require_ev_cli_version("0.0.13")

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
    find_package(date REQUIRED)
    find_package(nlohmann_json REQUIRED)
    find_package(nlohmann_json_schema_validator REQUIRED)
    find_package(PalSigslot REQUIRED)

    find_package(fmt REQUIRED)
    find_package(fsm REQUIRED)
    find_package(slac REQUIRED)

    find_package(everest-framework REQUIRED)
    find_package(everest-log REQUIRED)
    find_package(everest-sunspec REQUIRED)
    find_package(everest-modbus REQUIRED)
    find_package(everest-ocpp REQUIRED)

    find_package(pugixml REQUIRED)

    get_target_property(EVEREST_SCHEMA_DIR everest::framework EVEREST_DATADIR)
    set(EVEREST_SCHEMA_DIR "${EVEREST_SCHEMA_DIR}/schemas")
endif()

# source generate scripts / setup
include(everest-generate)

# auto generate headers
add_subdirectory(types)
add_subdirectory(interfaces)

# config
# FIXME (aw): this should be optional
add_subdirectory(config)

# modules
if (EVC_MAIN_PROJECT)
    add_subdirectory(modules)
endif()

# install types and interfaces
install(
    DIRECTORY "types" 
    DESTINATION "${CMAKE_INSTALL_DATADIR}/everest"
    FILES_MATCHING PATTERN "*.json"
)
install(
    DIRECTORY "interfaces"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/everest"
    FILES_MATCHING PATTERN "*.json"
)

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
if(BUILD_TESTING)
    include(CTest)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)

    evc_include(CodeCoverage)

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
