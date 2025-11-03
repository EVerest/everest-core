cmake_minimum_required(VERSION 3.11)

project(everest-timer
    VERSION 0.1.2
    DESCRIPTION "EVerest timer library"
    LANGUAGES CXX C
)

find_package(everest-cmake 0.1 REQUIRED
    PATHS ../everest-cmake
)

# options
option(${PROJECT_NAME}_BUILD_TESTING "Build unit tests, used if included as dependency" OFF)
option(BUILD_TESTING "Build unit tests, used if standalone project" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)
option(TIMER_INSTALL "Install the library (shared data might be installed anyway)" ${EVC_MAIN_PROJECT})
option(CMAKE_RUN_CLANG_TIDY "Run clang-tidy" OFF)

if((${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME} OR ${PROJECT_NAME}_BUILD_TESTING) AND BUILD_TESTING)
    set(LIBTIMER_BUILD_TESTING ON)
    include(CTest)
    add_subdirectory(tests)
endif()

# dependencies
find_package(Boost COMPONENTS system REQUIRED)

if (NOT DISABLE_EDM)
    evc_setup_edm()

    # In EDM mode, we can't install exports (because the dependencies usually do not install their exports)
    set(TIMER_INSTALL OFF)
else()
    find_package(date REQUIRED)
endif()


# logging library
add_subdirectory(lib)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# packaging
if (TIMER_INSTALL)
    install(
        TARGETS timer
        EXPORT timer-targets
    )

    install(
        DIRECTORY include/
        TYPE INCLUDE
    )

    evc_setup_package(
        NAME everest-timer
        EXPORT timer-targets
        NAMESPACE everest
        ADDITIONAL_CONTENT
            "find_dependency(Boost COMPONENTS system)"
    )
endif()

# build doxygen documentation if doxygen is available
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set( DOXYGEN_OUTPUT_DIRECTORY dist/docs )
    doxygen_add_docs(doxygen-${PROJECT_NAME} include lib src)
else()
    message("Doxygen is needed to generate documentation")
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
    -checks=${CLANG_TIDY_CHECKS})
endif()
