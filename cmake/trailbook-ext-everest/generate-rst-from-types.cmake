# This macro is for internal use only
#
# It is used in the function trailbook_ev_generate_rst_from_types().
# It adds an custom command to generate the RST file from the types definition file
macro(_trailbook_ev_generate_rst_from_types_generate_command)
    get_filename_component(TYPES_NAME ${args_TYPES_FILE} NAME_WE)
    set(GENERATED_FILE "${TRAILBOOK_EV_REFERENCE_TYPES_DIRECTORY}/${TYPES_NAME}.rst")
    set(TEMPLATES_DIRECTORY "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/templates")
    add_custom_command(
        OUTPUT
            ${GENERATED_FILE}
        DEPENDS
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/process_template.py
            ${args_TYPES_FILE}
            ${TEMPLATES_DIRECTORY}/types.rst.jinja
            ${TEMPLATES_DIRECTORY}/macros.jinja
        COMMENT
            "Generating RST file ${GENERATED_FILE} from types definition ${args_TYPES_FILE}"
        COMMAND
            ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/process_template.py
            --template-dir "${TEMPLATES_DIRECTORY}"
            --template-file "${TEMPLATES_DIRECTORY}/types.rst.jinja"
            --name "${TYPES_NAME}"
            --data-file "${args_TYPES_FILE}"
            --target-file "${GENERATED_FILE}"
    )
endmacro()


# This function generates an RST file from a types definition file.
# It takes the following arguments:
# TRAILBOOK_NAME (required):    The name of the trailbook.
# TYPES_FILE (required):        The absolute path to the types definition file.
# Usage:
# trailbook_ev_generate_rst_from_types(
#   TRAILBOOK_NAME <trailbook_name>
#   TYPES_FILE <absolute_path_to_types_definition_file>
# )
function(trailbook_ev_generate_rst_from_types)
    set(options)
    set(one_value_args
        TRAILBOOK_NAME
        TYPES_FILE
    )
    set(multi_value_args)
    cmake_parse_arguments(
        "args"
        "${options}"
        "${one_value_args}"
        "${multi_value_args}"
        ${ARGN}
    )

    # Parameter TRAILBOOK_NAME
    #   - is required
    #   - there should be a target named trailbook_<TRAILBOOK_NAME>
    if(NOT args_TRAILBOOK_NAME)
        message(FATAL_ERROR "trailbook_ext_ev_generate_rst_from_types: TRAILBOOK_NAME argument is required")
    endif()
    if(NOT TARGET trailbook_${args_TRAILBOOK_NAME})
        message(
            FATAL_ERROR
            "trailbook_ext_ev_generate_rst_from_types: No target named trailbook_${args_TRAILBOOK_NAME} found."
            " Did you forget to call add_trailbook() first?"
        )
    endif()

    # Parameter TYPES_FILE
    #   - is required
    #   - must be a absolute path
    #   - must exist
    if(NOT args_TYPES_FILE)
        message(FATAL_ERROR "trailbook_ext_ev_generate_rst_from_types: TYPES_FILE argument is required")
    endif()
    if(NOT IS_ABSOLUTE "${args_TYPES_FILE}")
        message(FATAL_ERROR "trailbook_ext_ev_generate_rst_from_types: TYPES_FILE must be an absolute path")
    endif()
    if(NOT EXISTS "${args_TYPES_FILE}")
        message(FATAL_ERROR "trailbook_ext_ev_generate_rst_from_types: TYPES_FILE must exist")
    endif()

    get_target_property(
        TRAILBOOK_INSTANCE_SOURCE_DIRECTORY
        trailbook_${args_TRAILBOOK_NAME}
        TRAILBOOK_INSTANCE_SOURCE_DIRECTORY
    )
    get_target_property(
        DEPS_STAGE_PREPARE_SPHINX_SOURCE_AFTER
        trailbook_${args_TRAILBOOK_NAME}
        DEPS_STAGE_PREPARE_SPHINX_SOURCE_AFTER
    )

    set(TRAILBOOK_EV_REFERENCE_DIRECTORY "${TRAILBOOK_INSTANCE_SOURCE_DIRECTORY}/reference")
    set(TRAILBOOK_EV_REFERENCE_TYPES_DIRECTORY "${TRAILBOOK_EV_REFERENCE_DIRECTORY}/types")

    _trailbook_ev_generate_rst_from_types_generate_command()

    add_custom_target(
        trailbook_${args_TRAILBOOK_NAME}_generate_rst_from_types_${TYPES_NAME}
        DEPENDS
            ${GENERATED_FILE}
            trailbook_${args_TRAILBOOK_NAME}_stage_prepare_sphinx_source_after
            ${DEPS_STAGE_PREPARE_SPHINX_SOURCE_AFTER}
        COMMENT
            "Target to generate RST file ${GENERATED_FILE} from types definition ${args_TYPES_FILE}"
    )
    set_property(
        TARGET
            trailbook_${args_TRAILBOOK_NAME}
        APPEND
        PROPERTY
            ADDITIONAL_DEPS_STAGE_BUILD_SPHINX_BEFORE
                ${GENERATED_FILE}
                trailbook_${args_TRAILBOOK_NAME}_generate_rst_from_types_${TYPES_NAME}
    )
endfunction()
