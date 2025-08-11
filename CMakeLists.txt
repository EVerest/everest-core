cmake_minimum_required(VERSION 3.14)

project(ocpp
    VERSION 0.30.1
    DESCRIPTION "A C++ implementation of the Open Charge Point Protocol"
    LANGUAGES CXX
)

find_package(everest-cmake 0.1 REQUIRED
    PATHS ../everest-cmake
)

# options
option(${PROJECT_NAME}_BUILD_TESTING "Build unit tests, used if included as dependency" OFF)
option(BUILD_TESTING "Build unit tests, used if standalone project" OFF)
option(CMAKE_RUN_CLANG_TIDY "Run clang-tidy" OFF)
option(LIBOCPP16_BUILD_EXAMPLES "Build charge_point binary" OFF)
option(OCPP_INSTALL "Install the library (shared data might be installed anyway)" ${EVC_MAIN_PROJECT})
option(LIBOCPP_ENABLE_DEPRECATED_WEBSOCKETPP "Websocket++ has been removed from the project" OFF)

option(LIBOCPP_ENABLE_V16 "Enable OCPP 1.6 in the ocpp library" ON)
option(LIBOCPP_ENABLE_V2 "Enable OCPP 2.0.1 and OCPP2.1 in the ocpp library" ON)

if((NOT LIBOCPP_ENABLE_V16) AND (NOT LIBOCPP_ENABLE_V2))
    message(FATAL_ERROR "At least one of LIBOCPP_ENABLE_V16 and LIBOCPP_ENABLE_V2 needs to be ON")
endif()

if(LIBOCPP_ENABLE_DEPRECATED_WEBSOCKETPP)
    message(FATAL_ERROR "Websocket++ has been removed")
endif()

if((${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME} OR ${PROJECT_NAME}_BUILD_TESTING) AND BUILD_TESTING)
    set(LIBOCPP_BUILD_TESTING ON)
    evc_include(CodeCoverage)

    append_coverage_compiler_flags()
endif()

# dependencies
find_package(Boost COMPONENTS program_options regex system thread REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(OpenSSL 3 REQUIRED)

if(NOT DISABLE_EDM)
    evc_setup_edm()

    # In EDM mode, we can't install exports (because the dependencies usually do not install their exports)
    set(OCPP_INSTALL OFF)
else()
    find_package(date REQUIRED)
    find_package(nlohmann_json REQUIRED)
    find_package(nlohmann_json_schema_validator REQUIRED)
    find_package(libwebsockets REQUIRED)
    find_package(fsm REQUIRED)
    find_package(everest-timer REQUIRED)
    find_package(everest-log REQUIRED)
    find_package(everest-evse_security REQUIRED)
    find_package(everest-sqlite REQUIRED)
endif()

# config and auxillary files
add_subdirectory(config/v16)
add_subdirectory(config/v2)

# library code
add_subdirectory(lib)

# packaging
if (OCPP_INSTALL)
    install(
        TARGETS ocpp
        EXPORT ocpp-targets
    )

    install(
        DIRECTORY include/
        TYPE INCLUDE
    )

    install(
        DIRECTORY 3rd_party/
        TYPE INCLUDE
    )

    evc_setup_package(
        NAME everest-ocpp
        NAMESPACE everest
        EXPORT ocpp-targets
        ADDITIONAL_CONTENT
            "find_dependency(OpenSSL)"
            "find_dependency(SQLite3)"
            "find_dependency(libwebsockets)"
            "find_dependency(everest-timer)"
            "find_dependency(everest-evse_security)"
            "find_dependency(everest-sqlite)"
    )
endif()


if(LIBOCPP16_BUILD_EXAMPLES)
    message("Building libocpp 1.6 example binaries.")
    add_subdirectory(src)
else()
    message("Not building libocpp 1.6 example binaries.")
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
    -checks=${CLANG_TIDY_CHECKS}
    -export-fixes=clang-tidy-fixes.yaml)
endif()

if(LIBOCPP_BUILD_TESTING)
    include(CTest)
    add_subdirectory(tests)
endif()

# build doxygen documentation if doxygen is available
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_OUTPUT_DIRECTORY dist/docs)
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)
    doxygen_add_docs(doxygen-${PROJECT_NAME} README.md include lib src doc)
else()
    message("Doxygen is needed to generate documentation")
endif()
