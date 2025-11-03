find_package(
    trailbook
    0.1.0
    REQUIRED
    PATHS "${CMAKE_SOURCE_DIR}/cmake"
)

include("${CMAKE_CURRENT_LIST_DIR}/add-module-explanation.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/generate-rst-from-interface.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/generate-rst-from-types.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/generate-rst-from-manifest.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/create-snapshot.cmake")
