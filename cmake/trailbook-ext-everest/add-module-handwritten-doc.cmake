# This macro is for internal use only
#
# It is used in the function trailbook_ev_add_module_handwritten_doc().
# It adds a custom command to copy the handwritten module files to the reference modules directory.
macro(_trailbook_ev_add_module_reference_copy_handwritten_command)
    file(
        GLOB_RECURSE
        MODULE_HANDWRITTEN_SOURCE_FILES
        CMAKE_CONFIGURE_DEPENDS
        "${args_HANDWRITTEN_DIR}/*"
    )

    set(MODULE_HANDWRITTEN_TARGET_FILES "")
    foreach(source_file IN LISTS MODULE_HANDWRITTEN_SOURCE_FILES)
        file(RELATIVE_PATH rel_path "${args_HANDWRITTEN_DIR}" "${source_file}")
        set(target_file "${TRAILBOOK_EV_HANDWRITTEN_MODULE_DOC_DIRECTORY}/${rel_path}")
        list(APPEND MODULE_HANDWRITTEN_TARGET_FILES "${target_file}")
    endforeach()

    add_custom_command(
        OUTPUT
            ${MODULE_HANDWRITTEN_TARGET_FILES}
        DEPENDS
            ${MODULE_HANDWRITTEN_SOURCE_FILES}
            ${DEPS_STAGE_PREPARE_SPHINX_SOURCE_AFTER}
        COMMENT
            "Copying handwritten documentation files of module ${args_MODULE_NAME} to: ${TRAILBOOK_EV_HANDWRITTEN_MODULE_DOC_DIRECTORY}/"
        COMMAND
            ${CMAKE_COMMAND} -E rm -rf
            ${MODULE_HANDWRITTEN_TARGET_FILES}
        COMMAND
            ${CMAKE_COMMAND} -E make_directory
            ${TRAILBOOK_EV_HANDWRITTEN_MODULE_DOC_DIRECTORY}/
        COMMAND
            ${CMAKE_COMMAND} -E copy_directory
            ${args_HANDWRITTEN_DIR}
            ${TRAILBOOK_EV_HANDWRITTEN_MODULE_DOC_DIRECTORY}/
    )
endmacro()

# This function adds a handwritten module documentation to a trailbook.
# It takes the following parameters:
#   TRAILBOOK_NAME (required):      The name of the trailbook to add the
#                                   documentation to.
#   MODULE_NAME (required):         The name of the module.
#   HANDWRITTEN_DIR (required):     The absolute path to the directory
#                                   containing the module's handwritten files.
#
# Usage:
# trailbook_ev_add_module_handwritten_doc(
#   TRAILBOOK_NAME <trailbook_name>
#   MODULE_NAME <module_name>
#   HANDWRITTEN_DIR <absolute_path_to_handwritten_docs_directory>
# )
function(trailbook_ev_add_module_handwritten_doc)
    set(options)
    set(one_value_args
        TRAILBOOK_NAME
        MODULE_NAME
        HANDWRITTEN_DIR
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
        message(FATAL_ERROR "trailbook_ev_add_module_handwritten_doc: TRAILBOOK_NAME argument is required")
    endif()
    if(NOT TARGET trailbook_${args_TRAILBOOK_NAME})
        message(
            FATAL_ERROR
            "trailbook_ev_add_module_handwritten_doc: No target named trailbook_${args_TRAILBOOK_NAME} found."
            " Did you forget to call add_trailbook() first?"
        )
    endif()

    # Parameter MODULE_NAME
    #   - is required
    if(NOT args_MODULE_NAME)
        message(FATAL_ERROR "trailbook_ev_add_module_handwritten_doc: MODULE_NAME argument is required")
    endif()

    # Parameter HANDWRITTEN_DIR
    #   - is required
    #   - must be a absolute path
    #   - must exist
    if(NOT args_HANDWRITTEN_DIR)
        message(FATAL_ERROR "trailbook_ev_add_module_handwritten_doc: HANDWRITTEN_DIR argument is required")
    endif()
    if(NOT IS_ABSOLUTE "${args_HANDWRITTEN_DIR}")
        message(FATAL_ERROR "trailbook_ev_add_module_handwritten_doc: HANDWRITTEN_DIR must be an absolute path")
    endif()
    if(NOT EXISTS "${args_HANDWRITTEN_DIR}")
        message(FATAL_ERROR "trailbook_ev_add_module_handwritten_doc: HANDWRITTEN_DIR does not exist")
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
    set(TRAILBOOK_EV_HANDWRITTEN_MODULE_DOC_DIRECTORY "${TRAILBOOK_EV_REFERENCE_DIRECTORY}/modules/${args_MODULE_NAME}")

    _trailbook_ev_add_module_reference_copy_handwritten_command()

    add_custom_target(
        trailbook_${args_TRAILBOOK_NAME}_handwritten_doc_module_${args_MODULE_NAME}
        DEPENDS            
            ${MODULE_HANDWRITTEN_TARGET_FILES}
        COMMENT
            "Handwritten documentation of module ${args_MODULE_NAME} for trailbook ${args_TRAILBOOK_NAME} is available."
    )
    set_property(
        TARGET
            trailbook_${args_TRAILBOOK_NAME}
        APPEND
        PROPERTY
            ADDITIONAL_DEPS_STAGE_BUILD_SPHINX_BEFORE
                ${MODULE_HANDWRITTEN_TARGET_FILES}
                trailbook_${args_TRAILBOOK_NAME}_handwritten_doc_module_${args_MODULE_NAME}
    )
endfunction()
