add_library(HashLibrary OBJECT
    hash_library/sha256.cpp
)

target_include_directories(HashLibrary PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

include(FetchContent)

FetchContent_Declare(
    libfmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 8.1.1
)

FetchContent_MakeAvailable(libfmt)
