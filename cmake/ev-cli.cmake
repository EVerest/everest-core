macro(setup_ev_cli)
    if(NOT TARGET ev-cli)
        add_custom_target(ev-cli)
    endif()
    if(NOT ${${PROJECT_NAME}_USE_PYTHON_VENV})
        message(STATUS "Using system ev-cli instead of installing it in the build venv.")
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
            OUTPUT_VARIABLE EV_DEV_TOOLS_PACKAGE_LOCATION
            OUTPUT_STRIP_TRAILING_WHITESPACE
            RESULT_VARIABLE
            EV_GET_PACKAGE_LOCATION_RESULT
        )

        message(STATUS "Using ev-dev-tool package: ${EV_DEV_TOOLS_PACKAGE_LOCATION}")

        find_program(EV_CLI ev-cli PATHS ${EV_ACTIVATE_PYTHON_VENV_PATH_TO_VENV}/bin REQUIRED)
        message(STATUS "Using ev-cli from: ${EV_CLI}")
    endif()

    get_property(EVEREST_REQUIRED_EV_CLI_VERSION
        GLOBAL
        PROPERTY EVEREST_REQUIRED_EV_CLI_VERSION
    )
    require_ev_cli_version(${EVEREST_REQUIRED_EV_CLI_VERSION})

    set_ev_cli_template_properties()
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

function(set_ev_cli_template_properties)
    message(STATUS "Setting template properties for ev-cli target")
    get_target_property(EVEREST_SCHEMA_DIR generate_cpp_files EVEREST_SCHEMA_DIR)

    execute_process(
        COMMAND ${EV_CLI} interface get-templates --separator=\; --schemas-dir "${EVEREST_SCHEMA_DIR}"
        OUTPUT_VARIABLE EV_CLI_INTERFACE_TEMPLATES
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE
        EV_CLI_INTERFACE_TEMPLATES_RESULT
    )

    if(EV_CLI_INTERFACE_TEMPLATES_RESULT)
        message(FATAL_ERROR "Could not get interface templates from ev-cli.")
    endif()

    execute_process(
        COMMAND ${EV_CLI} module get-templates --separator=\; --schemas-dir "${EVEREST_SCHEMA_DIR}"
        OUTPUT_VARIABLE EV_CLI_MODULE_TEMPLATES
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE
        EV_CLI_MODULE_TEMPLATES_RESULT
    )

    if(EV_CLI_MODULE_TEMPLATES_RESULT)
        message(FATAL_ERROR "Could not get module loader templates from ev-cli.")
    endif()

    execute_process(
        COMMAND ${EV_CLI} types get-templates --separator=\; --schemas-dir "${EVEREST_SCHEMA_DIR}"
        OUTPUT_VARIABLE EV_CLI_TYPES_TEMPLATES
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE
        EV_CLI_TYPES_TEMPLATES_RESULT
    )

    if(EV_CLI_TYPES_TEMPLATES_RESULT)
        message(FATAL_ERROR "Could not get module loader templates from ev-cli.")
    endif()

    set_target_properties(ev-cli
        PROPERTIES
            EV_CLI_INTERFACE_TEMPLATES "${EV_CLI_INTERFACE_TEMPLATES}"
            EV_CLI_MODULE_TEMPLATES "${EV_CLI_MODULE_TEMPLATES}"
            EV_CLI_TYPES_TEMPLATES "${EV_CLI_TYPES_TEMPLATES}"
    )
endfunction()
