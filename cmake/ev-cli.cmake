function(setup_ev_cli)
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

        get_target_property(SOURCE_DIRECTORY ev_pip_package_ev-dev-tools SOURCE_DIRECTORY)
        message(STATUS "Installing ev-cli from: ${SOURCE_DIRECTORY}")
        ev_pip_install_local(
            PACKAGE_NAME "ev-dev-tools"
            PACKAGE_SOURCE_DIRECTORY "${SOURCE_DIRECTORY}"
        )
        unset(EV_CLI CACHE)
        find_program(EV_CLI ev-cli HINTS ${EV_ACTIVATE_PYTHON_VENV_PATH_TO_VENV}/bin REQUIRED)
        message(STATUS "Using ev-cli from: ${EV_CLI}")
    endif()

    get_property(EVEREST_REQUIRED_EV_CLI_VERSION
        GLOBAL
        PROPERTY EVEREST_REQUIRED_EV_CLI_VERSION
    )
    require_ev_cli_version(${EVEREST_REQUIRED_EV_CLI_VERSION})

    set_ev_cli_template_properties()
endfunction()

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
        OUTPUT_VARIABLE INTERFACE_TEMPLATES
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE
        INTERFACE_TEMPLATES_RESULT
    )

    if(INTERFACE_TEMPLATES_RESULT)
        message(FATAL_ERROR "Could not get interface templates from ev-cli.")
    endif()

    execute_process(
        COMMAND ${EV_CLI} module get-templates --separator=\; --schemas-dir "${EVEREST_SCHEMA_DIR}"
        OUTPUT_VARIABLE MODULE_TEMPLATES
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE
        MODULE_TEMPLATES_RESULT
    )

    if(MODULE_TEMPLATES_RESULT)
        message(FATAL_ERROR "Could not get module loader templates from ev-cli.")
    endif()

    execute_process(
        COMMAND ${EV_CLI} types get-templates --separator=\; --schemas-dir "${EVEREST_SCHEMA_DIR}"
        OUTPUT_VARIABLE TYPES_TEMPLATES
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE
        TYPES_TEMPLATES_RESULT
    )

    if(TYPES_TEMPLATES_RESULT)
        message(FATAL_ERROR "Could not get module loader templates from ev-cli.")
    endif()

    set_target_properties(ev-cli
        PROPERTIES
            INTERFACE_TEMPLATES "${INTERFACE_TEMPLATES}"
            MODULE_TEMPLATES "${MODULE_TEMPLATES}"
            TYPES_TEMPLATES "${TYPES_TEMPLATES}"
    )
endfunction()
