find_package(
    trailbook
    0.1.0
    REQUIRED
    PATHS "${CMAKE_SOURCE_DIR}/cmake"
)
find_package(
    trailbook-ext-everest
    0.1.0
    REQUIRED
    PATHS "${CMAKE_SOURCE_DIR}/cmake"
)

add_trailbook(
    NAME "everest"
    STEM_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/source
    INSTANCE_NAME "${EVEREST_DOCS_VERSION_NAME}"
    DEPLOYED_DOCS_REPO_URL "https://github.com/everest/everest.github.io"
    DEPLOYED_DOCS_REPO_BRANCH "main"
)

get_target_property(
    TRAILBOOK_INSTANCE_SOURCE_DIRECTORY
    trailbook_everest
    TRAILBOOK_INSTANCE_SOURCE_DIRECTORY
)
get_filename_component(EVEREST_WORKSPACE_DIRECTORY ${CMAKE_SOURCE_DIR} DIRECTORY)
trailbook_ev_create_snapshot(
    EVEREST_WORKSPACE_DIRECTORY "${EVEREST_WORKSPACE_DIRECTORY}"
    TRAILBOOK_NAME "everest"
    OUTPUT_FILE "${TRAILBOOK_INSTANCE_SOURCE_DIRECTORY}/reference/assets/snapshot.yaml"
)
