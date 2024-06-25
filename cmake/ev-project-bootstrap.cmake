
# FIXME (aw): clean up this inclusion chain
include(${CMAKE_CURRENT_LIST_DIR}/ev-cli.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/config-run-script.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/config-run-nodered-script.cmake)

# dependencies
setup_ev_cli()
if(NOT ${${PROJECT_NAME}_INSTALL_EV_CLI_IN_PYTHON_VENV})
    require_ev_cli_version("0.2.0")
endif()

# source generate scripts / setup
include(${CMAKE_CURRENT_LIST_DIR}/everest-generate.cmake)
