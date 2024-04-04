
# FIXME (aw): clean up this inclusion chain
include(${CMAKE_CURRENT_LIST_DIR}/ev-cli.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/config-run-script.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/config-run-nodered-script.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/ev-version-information.cmake)

# dependencies
require_ev_cli_version("0.1.0")

# source generate scripts / setup
include(${CMAKE_CURRENT_LIST_DIR}/everest-generate.cmake)
