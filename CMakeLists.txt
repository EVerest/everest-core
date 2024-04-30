cmake_minimum_required(VERSION 3.14)

project(ocpp
    VERSION 0.11.0
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
option(LIBOCPP_BUILD_EXAMPLES "Build charge_point and central_system binaries." OFF)
option(OCPP_INSTALL "Install the library (shared data might be installed anyway)" ${EVC_MAIN_PROJECT})
option(LIBOCPP_ENABLE_DEPRECATED_WEBSOCKETPP "Usage of deprecated websocket++ instead of libwebsockets" OFF)

if((${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME} OR ${PROJECT_NAME}_BUILD_TESTING) AND BUILD_TESTING)
    set(LIBOCPP_BUILD_TESTING ON)
endif()

# dependencies
find_package(Boost COMPONENTS program_options regex system thread REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(OpenSSL REQUIRED)

if(NOT DISABLE_EDM)
    evc_setup_edm()

    # In EDM mode, we can't install exports (because the dependencies usually do not install their exports)
    set(OCPP_INSTALL OFF)

    if(LIBOCPP_ENABLE_DEPRECATED_WEBSOCKETPP)
        # FIXME (aw): websocketpp doesn't play well with EDM/CPM
        add_library(websocketpp::websocketpp INTERFACE IMPORTED)
        set_target_properties(websocketpp::websocketpp PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${websocketpp_SOURCE_DIR}")
    endif()
else()
    find_package(date REQUIRED)
    find_package(nlohmann_json REQUIRED)
    find_package(nlohmann_json_schema_validator REQUIRED)    
    find_package(libwebsockets REQUIRED)
    
    find_package(fsm REQUIRED)
    find_package(everest-timer REQUIRED)
    find_package(everest-log REQUIRED)
    find_package(everest-evse_security REQUIRED)

    if(LIBOCPP_ENABLE_DEPRECATED_WEBSOCKETPP)
        find_package(websocketpp REQUIRED)
    endif()
endif()

# config and auxillary files
add_subdirectory(config/v16)
add_subdirectory(config/v201)

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
    doxygen_add_docs(doxygen-${PROJECT_NAME} README.md include lib src)
else()
    message("Doxygen is needed to generate documentation")
endif()
