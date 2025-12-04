find_package(everest-cmake 0.6
    COMPONENTS bundling
    PATHS ../everest-cmake
    NO_DEFAULT_PATH
)
find_package(everest-cmake 0.6
    COMPONENTS bundling
)

if (NOT everest-cmake_FOUND)
    message(STATUS "Retrieving everest-cmake using FetchContent")
    include(FetchContent)
    FetchContent_Declare(
        everest-cmake
        GIT_REPOSITORY https://github.com/EVerest/everest-cmake.git
        GIT_TAG        v0.6.0
    )
    FetchContent_MakeAvailable(everest-cmake)
    set(everest-cmake_DIR "${everest-cmake_SOURCE_DIR}")
    set(everest-cmake_FIND_COMPONENTS "bundling")
    include("${everest-cmake_SOURCE_DIR}/everest-cmake-config.cmake")
endif()
