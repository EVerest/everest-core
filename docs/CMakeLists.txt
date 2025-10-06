find_package(
    trailbook
    0.1.0
    REQUIRED
    PATHS "${CMAKE_SOURCE_DIR}/cmake"
)

add_trailbook(
    NAME "everest"
    STEM_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/source
)
