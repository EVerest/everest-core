
# FIXME (aw): clean up this inclusion chain
include(${CMAKE_CURRENT_LIST_DIR}/ev-cli.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/config-run-script.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/config-run-nodered-script.cmake)

set_property(
    GLOBAL
    PROPERTY EVEREST_REQUIRED_EV_CLI_VERSION "0.2.0"
)

# source generate scripts / setup
include(${CMAKE_CURRENT_LIST_DIR}/everest-generate.cmake)
