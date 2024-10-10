macro(setup_ev_cli)
    if(NOT ${${PROJECT_NAME}_USE_PYTHON_VENV})
        find_program(EV_CLI ev-cli REQUIRED)
    else()
        ev_is_python_venv_active(
            RESULT_VAR IS_PYTHON_VENV_ACTIVE
        )
        if(NOT ${IS_PYTHON_VENV_ACTIVE})
            message(FATAL_ERROR "Python venv is not active. Please activate the python venv before running this command.")
        endif()

        find_program(EV_GET_PACKAGE_LOCATION
            NAMES get_package_location.py
            PATHS "${EVEREST_SCRIPTS_DIR}"
            NO_DEFAULT_PATH
        )
        execute_process(
            COMMAND ${Python3_EXECUTABLE} ${EV_GET_PACKAGE_LOCATION} --package-name ev-dev-tools
            OUTPUT_VARIABLE EV_CLI_PACKAGE_LOCATION
            OUTPUT_STRIP_TRAILING_WHITESPACE
            RESULT_VARIABLE
            EV_GET_PACKAGE_LOCATION_RESULT
        )
        if(EV_GET_PACKAGE_LOCATION_RESULT AND NOT EV_GET_PACKAGE_LOCATION_RESULT EQUAL 0)
            # TODO: this probably does not have to be a FATAL_ERROR
            message(FATAL_ERROR "Could not get ev-dev-tools package location")
        endif()
        message(STATUS "Using ev-cli package: ${EV_CLI_PACKAGE_LOCATION}")

        find_program(EV_CLI ev-cli PATHS ${EV_ACTIVATE_PYTHON_VENV_PATH_TO_VENV}/bin REQUIRED)

        set_property(
            GLOBAL
            PROPERTY EV_CLI_TEMPLATES_DIR "${EV_CLI_PACKAGE_LOCATION}/src/ev_cli/templates"
        )
    endif()
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
