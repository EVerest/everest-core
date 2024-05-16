macro(setup_ev_cli)
    add_custom_target(ev-cli)
    if(${EV_CLI})
        message(FATAL_ERROR "EV_CLI is already defined.")
        return()
    endif()
    if(NOT ${${PROJECT_NAME}_USE_PYTHON_VENV})
        find_program(EV_CLI ev-cli REQUIRED)
        return()
    endif()
    ev_is_python_venv_active(
        RESULT_VAR IS_PYTHON_VENV_ACTIVE
    )
    if(NOT ${IS_PYTHON_VENV_ACTIVE})
        message(FATAL_ERROR "Python venv is not active. Please activate the python venv before running this command.")
    endif()
    set(EV_CLI "${${PROJECT_NAME}_PYTHON_VENV_PATH}/bin/ev-cli")
    add_dependencies(ev-cli
        ev-dev-tools_pip_install_dist
    )
endmacro()

function(require_ev_cli_version EV_CLI_VERSION_REQUIRED)
    execute_process(
        COMMAND ${EV_CLI} --version
        OUTPUT_VARIABLE EV_CLI_VERSION_FULL
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    string(REPLACE "ev-cli " "" EV_CLI_VERSION "${EV_CLI_VERSION_FULL}")

    if ("${EV_CLI_VERSION}" STREQUAL "")
        message(FATAL_ERROR "Could not determine a ev-cli version from the provided version '${EV_CLI_VERSION_FULL}'")
    endif()
    if("${EV_CLI_VERSION}" VERSION_GREATER_EQUAL "${EV_CLI_VERSION_REQUIRED}")
        message("Found ev-cli version '${EV_CLI_VERSION}' which satisfies the requirement of ev-cli version '${EV_CLI_VERSION_REQUIRED}'")
    else()
        message(FATAL_ERROR "ev-cli version ${EV_CLI_VERSION_REQUIRED} or higher is required. However your ev-cli version is '${EV_CLI_VERSION}'. Please upgrade ev-cli.")
    endif()
endfunction()
