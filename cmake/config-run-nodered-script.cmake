function(generate_nodered_run_script)

    if (NOT EVEREST_ENABLE_RUN_SCRIPT_GENERATION)
        return ()
    endif ()

    set(options "")
    set(one_value_args
        FLOW
        OUTPUT
    )
    set(multi_value_args "")

    cmake_parse_arguments(OPTNS "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    if (OPTNS_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} got unknown argument(s): ${OPTNS_UNPARSED_ARGUMENTS}")
    endif()

    if (NOT OPTNS_FLOW)
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} requires FLOW parameter for the flow name")
    endif()

    set(FLOW_FILE "${CMAKE_CURRENT_SOURCE_DIR}/config-${OPTNS_FLOW}-flow.json")
    if (NOT EXISTS ${FLOW_FILE})
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION}: flow file '${FLOW_FILE}' does not exist")
    endif()

    set(SCRIPT_OUTPUT_PATH "${CMAKE_BINARY_DIR}/run-scripts")
    set(SCRIPT_OUTPUT_FILE "${SCRIPT_OUTPUT_PATH}/nodered-${OPTNS_FLOW}.sh")
    if (OPTNS_OUTPUT)
        set(SCRIPT_OUTPUT_FILE "${SCRIPT_OUTPUT_PATH}/nodered-${OPTNS_OUTPUT}.sh")
    endif()

    configure_file("${EVEREST_CONFIG_ASSET_DIR}/run_nodered_template.sh.in" ${SCRIPT_OUTPUT_FILE})

endfunction()
