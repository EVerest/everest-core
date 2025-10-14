find_package(
    trailbook
    0.1.0
    REQUIRED
    PATHS "${CMAKE_SOURCE_DIR}/cmake"
)

add_trailbook(
    NAME "everest"
    STEM_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/source
    INSTANCE_NAME "local"
    DEPLOYED_DOCS_REPO_URL "https://github.com/everest/everest.github.io"
    DEPLOYED_DOCS_REPO_BRANCH "main"
)
