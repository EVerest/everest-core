set_property(
    GLOBAL
    PROPERTY EVEREST_REQUIRED_EV_CLI_VERSION "0.5.2"
)

# FIXME (aw): clean up this inclusion chain
include(${CMAKE_CURRENT_LIST_DIR}/ev-cli.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/config-run-script.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/config-run-nodered-script.cmake)

# source generate scripts / setup
include(${CMAKE_CURRENT_LIST_DIR}/ev-targets.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/everest-generate.cmake)
