cmake_minimum_required(VERSION 3.14)

project(cb_v2g
    VERSION 0.3.1
    DESCRIPTION "V2GTP EXI library"
    HOMEPAGE_URL "https://github.com/Everest/libcbv2g"
	LANGUAGES C CXX
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
option(CB_V2G_INSTALL "Install the library (shared data might be installed anyway)" ${EVC_MAIN_PROJECT})

add_subdirectory(lib)

message(STATUS "library code based on cbexigen generator version: 871fcb3")

# tests
if (CB_V2G_BUILD_TESTS)
    include(CTest)
    add_subdirectory(tests)
endif()

# packaging
if (CB_V2G_INSTALL)
    install(
        TARGETS
            cbv2g_exi_codec
            cbv2g_din
            cbv2g_iso2
            cbv2g_iso20
            cbv2g_tp
        EXPORT cbv2g-targets
        LIBRARY
    )

    install(
        DIRECTORY include/
        TYPE INCLUDE
    )

    evc_setup_package(
        NAME cbv2g
        NAMESPACE cbv2g
        EXPORT cbv2g-targets
    )
endif()

