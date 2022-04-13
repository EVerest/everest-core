cmake_minimum_required(VERSION 3.14.7)

project(OCPP VERSION 0.1
             DESCRIPTION "A C++ implementation of the Open Charge Point Protocol"
             LANGUAGES CXX)

# enable compiler warnings
add_compile_options(-Wall -Wextra -pedantic)
# unused functions in a library should not be problematic
add_compile_options(-Wno-unused)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
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
    -checks=${CLANG_TIDY_CHECKS}
    -export-fixes=clang-tidy-fixes.yaml)
endif()

# build doxygen documentation if doxygen is available
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_OUTPUT_DIRECTORY dist/docs)
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)
    doxygen_add_docs(${PROJECT_NAME}doxygen README.md include lib src)
else()
    message("Doxygen is needed to generate documentation")
endif()

add_definitions(-DBOOST_LOG_DYN_LINK)

find_package(EDM REQUIRED)

find_package(Boost COMPONENTS filesystem program_options regex system thread REQUIRED)

find_package(SQLite3 REQUIRED)

find_package(OpenSSL REQUIRED)

# library code
add_subdirectory(lib)

# executable code
option(LIBOCPP_BUILD_EXAMPLES "Build charge_point and central_system binaries." OFF)
if(LIBOCPP_BUILD_EXAMPLES)
    message("Building libocpp example binaries.")
    add_subdirectory(src)
else()
    message("Not building libocpp example binaries.")
endif()

# auxillary files
add_subdirectory(aux)
