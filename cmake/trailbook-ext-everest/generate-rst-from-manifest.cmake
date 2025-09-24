# This macro is for internal use only
#
# It is used in the function trailbook_ev_generate_rst_from_manifest().
# It adds an custom command to generate the RST file from the manifest file
macro(_trailbook_ev_generate_rst_from_manifest_generate_command)
    set(GENERATED_FILE "${TRAILBOOK_EV_REFERENCE_MODULES_DIRECTORY}/${MODULE_NAME}.rst")
    set(TEMPLATES_DIRECTORY "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/templates")
    if(EXISTS "${MODULE_DIR}/docs/")
        set(HAS_MODULE_EXPLANATION "--has-module-explanation")
    endif()
    add_custom_command(
        OUTPUT
            ${GENERATED_FILE}
        DEPENDS
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/process_template.py
            ${args_MANIFEST_FILE}
            ${TEMPLATES_DIRECTORY}/module.rst.jinja
            ${TEMPLATES_DIRECTORY}/macros.jinja
            trailbook_${args_TRAILBOOK_NAME}_stage_prepare_sphinx_source_after
            ${DEPS_STAGE_PREPARE_SPHINX_SOURCE_AFTER}
        COMMENT
            "Generating RST file ${GENERATED_FILE} from manifest ${args_MANIFEST_FILE}"
        COMMAND
            ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/process_template.py
            --template-dir "${TEMPLATES_DIRECTORY}"
            --template-file "${TEMPLATES_DIRECTORY}/module.rst.jinja"
            --name "${MODULE_NAME}"
            --data-file "${args_MANIFEST_FILE}"
            --target-file "${GENERATED_FILE}"
            ${HAS_MODULE_EXPLANATION}
    )
endmacro()


# This function generates an RST file from a manifest definition file.
# It takes the following arguments:
# TRAILBOOK_NAME (required):    The name of the trailbook.
# MANIFEST_FILE (required):     The absolute path to the manifest 
#                               definition file.
# Usage:
# trailbook_ev_generate_rst_from_manifest(
#     TRAILBOOK_NAME <trailbook_name>
#     MANIFEST_FILE <path_to_manifest_definition_file>
# )
function(trailbook_ev_generate_rst_from_manifest)
    set(options)
    set(one_value_args
        TRAILBOOK_NAME
        MANIFEST_FILE
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
        message(FATAL_ERROR "trailbook_ext_ev_generate_rst_from_manifest: TRAILBOOK_NAME argument is required")
    endif()
    if(NOT TARGET trailbook_${args_TRAILBOOK_NAME})
        message(
            FATAL_ERROR
            "trailbook_ext_ev_generate_rst_from_manifest: No target named trailbook_${args_TRAILBOOK_NAME} found."
            " Did you forget to call add_trailbook() first?"
        )
    endif()

    # Parameter MANIFEST_FILE
    #   - is required
    #   - must be a absolute path
    #   - must exist
    if(NOT args_MANIFEST_FILE)
        message(FATAL_ERROR "trailbook_ext_ev_generate_rst_from_manifest: MANIFEST_FILE argument is required")
    endif()
    if(NOT IS_ABSOLUTE "${args_MANIFEST_FILE}")
        message(FATAL_ERROR "trailbook_ext_ev_generate_rst_from_manifest: MANIFEST_FILE must be an absolute path")
    endif()
    if(NOT EXISTS "${args_MANIFEST_FILE}")
        message(FATAL_ERROR "trailbook_ext_ev_generate_rst_from_manifest: MANIFEST_FILE must exist")
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
    set(TRAILBOOK_EV_REFERENCE_MODULES_DIRECTORY "${TRAILBOOK_EV_REFERENCE_DIRECTORY}/modules")
    get_filename_component(MODULE_DIR ${args_MANIFEST_FILE} DIRECTORY)
    get_filename_component(MODULE_NAME ${MODULE_DIR} NAME_WE)


    _trailbook_ev_generate_rst_from_manifest_generate_command()

    add_custom_target(
        trailbook_${args_TRAILBOOK_NAME}_generate_rst_from_manifest_${MODULE_NAME}
        DEPENDS
            ${GENERATED_FILE}
            trailbook_${args_TRAILBOOK_NAME}_stage_prepare_sphinx_source_after
            ${DEPS_STAGE_PREPARE_SPHINX_SOURCE_AFTER}
        COMMENT
            "Target to generate RST file ${GENERATED_FILE} from manifest definition ${args_MANIFEST_FILE}"
    )
    set_property(
        TARGET
            trailbook_${args_TRAILBOOK_NAME}
        APPEND
        PROPERTY
            ADDITIONAL_DEPS_STAGE_BUILD_SPHINX_BEFORE
                ${GENERATED_FILE}
                trailbook_${args_TRAILBOOK_NAME}_generate_rst_from_manifest_${MODULE_NAME}
    )
endfunction()
