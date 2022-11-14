cmake_minimum_required(VERSION 3.14)

project(everest-framework
    VERSION 0.1
    DESCRIPTION "The open operating system for e-mobility charging stations"
	LANGUAGES CXX C
)

find_package(everest-cmake 0.1 REQUIRED
    PATHS ../everest-cmake
)

# options
option(BUILD_TESTING "Run unit tests" OFF)
option(FRAMEWORK_INSTALL "Install the library (shared data might be installed anyway)" ${EVC_MAIN_PROJECT})
option(CMAKE_RUN_CLANG_TIDY "Run clang-tidy" OFF)

# make own cmake modules available
list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# dependencies
find_package(Boost
    COMPONENTS
        program_options
        system
        thread
    REQUIRED
)

if(NOT DISABLE_EDM)
    evc_setup_edm()

    # In EDM mode, we can't install exports (because the dependencies usually do not install their exports)
    set(FRAMEWORK_INSTALL OFF)

    # FIXME (aw): websocketpp doesn't play well with EDM/CPM
    add_library(websocketpp::websocketpp INTERFACE IMPORTED)
    set_target_properties(websocketpp::websocketpp PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${websocketpp_SOURCE_DIR}")

    # FIXME (aw): pboetch json validator doesn't want to install itself :(
    install(TARGETS nlohmann_json_schema_validator
        EXPORT nlohmann_json_schema_validatorTargets
    )

    # FIXME (aw): add catch2's cmake folder
    list(APPEND CMAKE_MODULE_PATH "${Catch2_SOURCE_DIR}/contrib")
else()
    find_package(date REQUIRED)
    find_package(ryml REQUIRED)
    find_package(nlohmann_json REQUIRED)
    find_package(nlohmann_json_schema_validator REQUIRED)
    find_package(websocketpp REQUIRED)
    find_package(fmt REQUIRED)
    find_package(everest-log REQUIRED)

    if (BUILD_TESTING)
        find_package(Catch2 REQUIRED)
    endif()

    include(find-mqttc)
endif()

set(EVEREST_FRAMEWORK_GENERATED_INC_DIR ${PROJECT_BINARY_DIR}/generated)
configure_file(
    include/compile_time_settings.hpp.in
    ${EVEREST_FRAMEWORK_GENERATED_INC_DIR}/everest/compile_time_settings.hpp
)

# library code
add_subdirectory(lib)

# executable code
add_subdirectory(src)

# auxillary files
add_subdirectory(schemas)

# everest javascript wrapper
add_subdirectory(everestjs)

# FIXME (aw): should this be installed or not?  Right now it is needed for the
#             current packaging approach
install(TARGETS framework
    EXPORT framework-targets
    LIBRARY
)

# packaging
if (FRAMEWORK_INSTALL)
    install(
        DIRECTORY include/
        DESTINATION include/everest
    )

    evc_setup_package(
        NAME everest-framework
        EXPORT framework-targets
        NAMESPACE everest
        ADDITIONAL_CONTENT
            "set_target_properties(everest::framework PROPERTIES EVEREST_DATADIR \"@PACKAGE_EVEREST_DATADIR@\")"
            "set(EVEREST_DATADIR \"@PACKAGE_EVEREST_DATADIR@\")"
        PATH_VARS
            EVEREST_DATADIR "${CMAKE_INSTALL_DATADIR}/everest"
    )
endif ()

# testing
# FIXME (aw): move testing logic into tests/CMakeLists.txt
if(BUILD_TESTING)
    include(CTest)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)

    evc_include(CodeCoverage)

    append_coverage_compiler_flags()

    add_subdirectory(tests)

    setup_target_for_coverage_gcovr_html(
        NAME gcovr_coverage
        EXECUTABLE ctest --output-on-failure
        EXCLUDE "${CMAKE_SOURCE_DIR}/tests" "${CMAKE_BINARY_DIR}"
        DEPENDENCIES tests everest
    )

    setup_target_for_coverage_lcov(
        NAME lcov_coverage
        EXECUTABLE ctest --output-on-failure
        EXCLUDE "${CMAKE_SOURCE_DIR}/tests" "${CMAKE_BINARY_DIR}"
        DEPENDENCIES tests everest
    )
else()
    message("Not running unit tests")
endif()

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
else()
    message("Doxygen is needed to generate documentation")
endif()
