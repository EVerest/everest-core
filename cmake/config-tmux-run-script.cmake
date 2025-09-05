# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

set(EVEREST_CONFIG_ASSET_DIR "${CMAKE_CURRENT_LIST_DIR}/assets" CACHE INTERNAL "")

function(generate_config_tmux_run_script)

    if (NOT EVEREST_ENABLE_RUN_SCRIPT_GENERATION)
        return ()
    endif ()

    set(options "")
    set(one_value_args
        CONFIG
        LOGGING_CONFIG
        OUTPUT
    )
    set(multi_value_args
        ADDITIONAL_ARGUMENTS
    )

    cmake_parse_arguments(OPTNS "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    if (OPTNS_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} got unknown argument(s): ${OPTNS_UNPARSED_ARGUMENTS}")
    endif()

    if (NOT OPTNS_CONFIG)
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} requires CONFIG parameter for the config name")
    endif()

    set(HELPER_PREFIX ${CMAKE_INSTALL_PREFIX}/etc/everest)

    set(CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/config-${OPTNS_CONFIG}.yaml")
    if (NOT EXISTS ${CONFIG_FILE})
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION}: config file '${CONFIG_FILE}' does not exist")
    endif()

    set(LOGGING_CONFIG_FILE "${EVEREST_CONFIG_ASSET_DIR}/logging.ini")
    if (OPTNS_LOGGING_CONFIG)
        set(LOGGING_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${OPTNS_LOGGING_CONFIG}.ini")
    endif()

    if (NOT EXISTS ${LOGGING_CONFIG_FILE})
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION}: logging config file '${LOGGING_CONFIG_FILE}' does not exist")
    endif()

    set(SCRIPT_OUTPUT_PATH "${CMAKE_BINARY_DIR}/run-scripts")
    set(SCRIPT_OUTPUT_FILE "${SCRIPT_OUTPUT_PATH}/run-tmux-${OPTNS_CONFIG}.sh")
    if (OPTNS_OUTPUT)
        set(SCRIPT_OUTPUT_FILE "${SCRIPT_OUTPUT_PATH}/run-tmux-${OPTNS_OUTPUT}.sh")
    endif()

    # other necessary variables
    set(LD_LIBRARY_VAR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
    set(PATH_VAR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")

    configure_file("${EVEREST_CONFIG_ASSET_DIR}/run_tmux_template.sh.in" ${SCRIPT_OUTPUT_FILE})

endfunction()
