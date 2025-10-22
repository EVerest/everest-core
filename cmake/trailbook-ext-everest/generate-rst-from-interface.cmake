# This macro is for internal use only
#
# It is used in the function trailbook_ev_generate_rst_from_interface().
# It adds an custom command to generate the RST file from the interface definition file
macro(_trailbook_ev_generate_rst_from_interface_generate_command)
    get_filename_component(INTERFACE_NAME ${args_INTERFACE_FILE} NAME_WE)
    set(GENERATED_FILE "${TRAILBOOK_EV_REFERENCE_INTERFACES_DIRECTORY}/${INTERFACE_NAME}.rst")
    set(TEMPLATES_DIRECTORY "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/templates")
    add_custom_command(
        OUTPUT
            ${GENERATED_FILE}
        DEPENDS
            trailbook_${args_TRAILBOOK_NAME}_stage_prepare_sphinx_source_after
            ${DEPS_STAGE_PREPARE_SPHINX_SOURCE_AFTER}
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/process_template.py
            ${args_INTERFACE_FILE}
            ${TEMPLATES_DIRECTORY}/interface.rst.jinja
            ${TEMPLATES_DIRECTORY}/macros.jinja
        COMMENT
            "Generating RST file ${GENERATED_FILE} from interface definition ${args_INTERFACE_FILE}"
        COMMAND
            ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/process_template.py
            --template-dir "${TEMPLATES_DIRECTORY}"
            --template-file "${TEMPLATES_DIRECTORY}/interface.rst.jinja"
            --name "${INTERFACE_NAME}"
            --data-file "${args_INTERFACE_FILE}"
            --target-file "${GENERATED_FILE}"
    )
endmacro()


# This function generates an RST file from an interface definition file.
#
# Arguments:
#   TRAILBOOK_NAME (required): Name of the trailbook instance.
#   INTERFACE_FILE (required): Path to the interface definition file
# Usage:
# trailbook_ev_generate_rst_from_interface(
#     TRAILBOOK_NAME <trailbook_name>
#     INTERFACE_FILE <path_to_interface_definition_file>
# )
function(trailbook_ev_generate_rst_from_interface)
    set(options)
    set(one_value_args
        TRAILBOOK_NAME
        INTERFACE_FILE
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
        message(FATAL_ERROR "trailbook_ext_ev_generate_rst_from_interface: TRAILBOOK_NAME argument is required")
    endif()
    if(NOT TARGET trailbook_${args_TRAILBOOK_NAME})
        message(
            FATAL_ERROR
            "trailbook_ext_ev_generate_rst_from_interface: No target named trailbook_${args_TRAILBOOK_NAME} found."
            " Did you forget to call add_trailbook() first?"
        )
    endif()

    # Parameter INTERFACE_FILE
    #   - is required
    #   - must be a absolute path
    #   - must exist
    if(NOT args_INTERFACE_FILE)
        message(FATAL_ERROR "trailbook_ext_ev_generate_rst_from_interface: INTERFACE_FILE argument is required")
    endif()
    if(NOT IS_ABSOLUTE "${args_INTERFACE_FILE}")
        message(FATAL_ERROR "trailbook_ext_ev_generate_rst_from_interface: INTERFACE_FILE must be an absolute path")
    endif()
    if(NOT EXISTS "${args_INTERFACE_FILE}")
        message(FATAL_ERROR "trailbook_ext_ev_generate_rst_from_interface: INTERFACE_FILE must exist")
    endif()

    get_target_property(
        TRAILBOOK_INSTANCE_SOURCE_DIRECTORY
        trailbook_${args_TRAILBOOK_NAME}
        TRAILBOOK_INSTANCE_SOURCE_DIRECTORY
    )
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


    set(TRAILBOOK_EV_REFERENCE_DIRECTORY "${TRAILBOOK_INSTANCE_SOURCE_DIRECTORY}/reference")
    set(TRAILBOOK_EV_REFERENCE_INTERFACES_DIRECTORY "${TRAILBOOK_EV_REFERENCE_DIRECTORY}/interfaces")


    _trailbook_ev_generate_rst_from_interface_generate_command()

    add_custom_target(
        trailbook_${args_TRAILBOOK_NAME}_generate_rst_from_interface_${INTERFACE_NAME}
        DEPENDS
            ${GENERATED_FILE}
            trailbook_${args_TRAILBOOK_NAME}_stage_prepare_sphinx_source_after
            ${DEPS_STAGE_PREPARE_SPHINX_SOURCE_AFTER}
        COMMENT
            "Target to generate RST file ${GENERATED_FILE} from interface definition ${args_INTERFACE_FILE}"
    )
    set_property(
        TARGET
            trailbook_${args_TRAILBOOK_NAME}
        APPEND
        PROPERTY
            ADDITIONAL_DEPS_STAGE_BUILD_SPHINX_BEFORE
                ${GENERATED_FILE}
                trailbook_${args_TRAILBOOK_NAME}_generate_rst_from_interface_${INTERFACE_NAME}
    )
endfunction()
