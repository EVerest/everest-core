if(NOT DEFINED _TRAILBOOK_EXT_EVEREST_CREATE_SNAPSHOT_SETUP)
    if(NOT DEFINED everest-utils_SOURCE_DIR)
        message(FATAL_ERROR "everest-utils not found. Did you forget to add it to your dependencies.yaml?")
    endif()
    set(_TRAILBOOK_EXT_EVEREST_CREATE_SNAPSHOT_SCRIPT
        "${everest-utils_SOURCE_DIR}/scripts/create_snapshot.py"
    )
    if(NOT EXISTS "${_TRAILBOOK_EXT_EVEREST_CREATE_SNAPSHOT_SCRIPT}")
        message(FATAL_ERROR "everest-utils found, but create_snapshot.py script is missing at ${_TRAILBOOK_EXT_EVEREST_CREATE_SNAPSHOT_SCRIPT}")
    endif()
    set(_TRAILBOOK_EXT_EVEREST_CREATE_SNAPSHOT_SETUP TRUE)
endif()


# This function creates a snapshot file and adds it
# to the given trailbook
# Parameters:
#   EVEREST_WORKSPACE_DIRECTORY (required):     Absolute path to the EVerest workspace
#                                               directory
#   TRAILBOOK_NAME (required):                  Name of the trailbook (the
#                                               target must exist)
#   OUTPUT_FILE (required):                     Absolute path to the output
#                                               snapshot file
# Usage:
# trailbook_ev_create_snapshot(
#   EVEREST_WORKSPACE_DIRECTORY <path_to_everest_workspace_directory>
#   TRAILBOOK_NAME <trailbook_name>
#   OUTPUT_FILE <absolute_path_to_output_snapshot_file>
# )
function(trailbook_ev_create_snapshot)
    set(options)
    set(one_value_args
        EVEREST_WORKSPACE_DIRECTORY
        TRAILBOOK_NAME
        OUTPUT_FILE
    )
    set(multi_value_args)
    cmake_parse_arguments(
        "args"
        "${options}"
        "${one_value_args}"
        "${multi_value_args}"
        ${ARGN}
    )

    # Parameter EVEREST_WORKSPACE_DIRECTORY
    #   - is required
    #   - must be a absolute path
    #   - must exist
    if(NOT EVEREST_WORKSPACE_DIRECTORY)
        message(FATAL_ERROR "trailbook_ev_create_snapshot: EVEREST_WORKSPACE_DIRECTORY argument is required")
    endif()
    if(NOT IS_ABSOLUTE "${EVEREST_WORKSPACE_DIRECTORY}")
        message(FATAL_ERROR "trailbook_ev_create_snapshot: EVEREST_WORKSPACE_DIRECTORY must be an absolute path")
    endif()
    if(NOT EXISTS "${EVEREST_WORKSPACE_DIRECTORY}")
        message(FATAL_ERROR "trailbook_ev_create_snapshot: EVEREST_WORKSPACE_DIRECTORY must exist")
    endif()

    # Parameter TRAILBOOK_NAME
    #   - is required
    #   - there should be a target named trailbook_<TRAILBOOK_NAME>
    if(NOT args_TRAILBOOK_NAME)
        message(FATAL_ERROR "trailbook_ev_create_snapshot: TRAILBOOK_NAME argument is required")
    endif()
    if(NOT TARGET trailbook_${args_TRAILBOOK_NAME})
        message(
            FATAL_ERROR
            "trailbook_ev_create_snapshot: No target named trailbook_${args_TRAILBOOK_NAME} found."
            " Did you forget to call add_trailbook() first?"
        )
    endif()

    # Parameter OUTPUT_FILE
    #   - is required
    #   - must be a absolute path
    if(NOT args_OUTPUT_FILE)
        message(FATAL_ERROR "trailbook_ev_create_snapshot: OUTPUT_FILE argument is required")
    endif()
    if(NOT IS_ABSOLUTE "${args_OUTPUT_FILE}")
        message(FATAL_ERROR "trailbook_ev_create_snapshot: OUTPUT_FILE must be an absolute path")
    endif()

    get_target_property(
        TRAILBOOK_CURRENT_BINARY_DIR
        trailbook_${args_TRAILBOOK_NAME}
        TRAILBOOK_CURRENT_BINARY_DIR
    )
    get_target_property(
        DEPS_STAGE_PREPARE_SPHINX_SOURCE_AFTER
        trailbook_${args_TRAILBOOK_NAME}
        DEPS_STAGE_PREPARE_SPHINX_SOURCE_AFTER
    )
    set(CREATE_SNAPSHOT_TEMP_DIR "${TRAILBOOK_CURRENT_BINARY_DIR}/create_snapshot_temp")
    add_custom_command(
        OUTPUT
            ${args_OUTPUT_FILE}
        DEPENDS
            ${_TRAILBOOK_EXT_EVEREST_CREATE_SNAPSHOT_SCRIPT}
            trailbook_${args_TRAILBOOK_NAME}_stage_prepare_sphinx_source_after
            ${DEPS_STAGE_PREPARE_SPHINX_SOURCE_AFTER}
        USES_TERMINAL
        COMMAND
            ${CMAKE_COMMAND} -E rm -rf
            ${CREATE_SNAPSHOT_TEMP_DIR}
        COMMAND
            ${Python3_EXECUTABLE}
            ${_TRAILBOOK_EXT_EVEREST_CREATE_SNAPSHOT_SCRIPT}
            --working-dir ${args_EVEREST_WORKSPACE_DIRECTORY}
            --temp-dir ${CREATE_SNAPSHOT_TEMP_DIR}
            --allow-relative-to-working-dir
            --exclude-dir build/
            --exclude-dir .vscode/
            --exclude-dir dist/
            --exclude-dir cache/
            --exclude-dir scripts/
        COMMAND
            ${CMAKE_COMMAND} -E copy
            ${CREATE_SNAPSHOT_TEMP_DIR}/snapshot.yaml
            ${args_OUTPUT_FILE}
    )
    add_custom_target(
        trailbook_${args_TRAILBOOK_NAME}_create_snapshot
        DEPENDS
            trailbook_${args_TRAILBOOK_NAME}_stage_prepare_sphinx_source_after
            ${DEPS_STAGE_PREPARE_SPHINX_SOURCE_AFTER}
            ${args_OUTPUT_FILE}
        COMMENT
            "Target to create snapshot file ${args_OUTPUT_FILE} for trailbook ${args_TRAILBOOK_NAME}"
    )
    set_property(
        TARGET
            trailbook_${args_TRAILBOOK_NAME}
        APPEND
        PROPERTY
            ADDITIONAL_DEPS_STAGE_BUILD_SPHINX_BEFORE
            ${args_OUTPUT_FILE}
            trailbook_${args_TRAILBOOK_NAME}_create_snapshot
    )
endfunction()
