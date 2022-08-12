add_library(HashLibrary OBJECT
    hash_library/sha256.cpp
)

target_include_directories(HashLibrary PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
