# This macro is for internal use only
#
# It is used in the function trailbook_ev_add_module_explanation().
# It adds a custom command to copy the explanation module files to the explanation modules directory.
macro(_trailbook_ev_add_module_explanation_copy_explanation_command)
    file(
        GLOB_RECURSE
        MODULE_EXPLANATION_SOURCE_FILES
        CMAKE_CONFIGURE_DEPENDS
        "${args_EXPLANATION_DIR}/*"
    )

    set(MODULE_EXPLANATION_TARGET_FILES "")
    foreach(source_file IN LISTS MODULE_EXPLANATION_SOURCE_FILES)
        file(RELATIVE_PATH rel_path "${args_EXPLANATION_DIR}" "${source_file}")
        set(target_file "${TRAILBOOK_EV_EXPLANATION_MODULES_DIRECTORY}/${args_MODULE_NAME}/${rel_path}")
        list(APPEND MODULE_EXPLANATION_TARGET_FILES "${target_file}")
    endforeach()

    add_custom_command(
        OUTPUT
            ${MODULE_EXPLANATION_TARGET_FILES}
        DEPENDS
            ${MODULE_EXPLANATION_SOURCE_FILES}
            ${DEPS_STAGE_PREPARE_SPHINX_SOURCE_AFTER}
        COMMENT
            "Copying explanation module ${args_MODULE_NAME} files to: ${TRAILBOOK_EV_EXPLANATION_MODULES_DIRECTORY}/${args_MODULE_NAME}"
        COMMAND
            ${CMAKE_COMMAND} -E rm -rf
            ${MODULE_EXPLANATION_TARGET_FILES}
        COMMAND
            ${CMAKE_COMMAND} -E make_directory
            ${TRAILBOOK_EV_EXPLANATION_MODULES_DIRECTORY}/${args_MODULE_NAME}
        COMMAND
            ${CMAKE_COMMAND} -E copy_directory
            ${args_EXPLANATION_DIR}
            ${TRAILBOOK_EV_EXPLANATION_MODULES_DIRECTORY}/${args_MODULE_NAME}
    )
endmacro()

# This function adds a module explanation to a trailbook.
# It takes the following parameters:
#   TRAILBOOK_NAME (required):      The name of the trailbook to add the
#                                   module explanation to.
#   MODULE_NAME (required):         The name of the module explanation.
#   EXPLANATION_DIR (required):     The absolute path to the directory
#                                   containing the module explanation files.
#
# Usage:
# trailbook_ev_add_module_explanation(
#   TRAILBOOK_NAME <trailbook_name>
#   MODULE_NAME <module_name>
#   EXPLANATION_DIR <absolute_path_to_explanation_directory>
# )
function(trailbook_ev_add_module_explanation)
    set(options)
    set(one_value_args
        TRAILBOOK_NAME
        MODULE_NAME
        EXPLANATION_DIR
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
        message(FATAL_ERROR "trailbook_ev_add_module_explanation: TRAILBOOK_NAME argument is required")
    endif()
    if(NOT TARGET trailbook_${args_TRAILBOOK_NAME})
        message(
            FATAL_ERROR
            "trailbook_ev_add_module_explanation: No target named trailbook_${args_TRAILBOOK_NAME} found."
            " Did you forget to call add_trailbook() first?"
        )
    endif()

    # Parameter MODULE_NAME
    #   - is required
    if(NOT args_MODULE_NAME)
        message(FATAL_ERROR "trailbook_ev_add_module_explanation: MODULE_NAME argument is required")
    endif()

    # Parameter EXPLANATION_DIR
    #   - is required
    #   - must be a absolute path
    #   - must exist
    if(NOT args_EXPLANATION_DIR)
        message(FATAL_ERROR "trailbook_ev_add_module_explanation: EXPLANATION_DIR argument is required")
    endif()
    if(NOT IS_ABSOLUTE "${args_EXPLANATION_DIR}")
        message(FATAL_ERROR "trailbook_ev_add_module_explanation: EXPLANATION_DIR must be an absolute path")
    endif()
    if(NOT EXISTS "${args_EXPLANATION_DIR}")
        message(FATAL_ERROR "trailbook_ev_add_module_explanation: EXPLANATION_DIR does not exist")
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

    set(TRAILBOOK_EV_EXPLANATION_DIRECTORY "${TRAILBOOK_INSTANCE_SOURCE_DIRECTORY}/explanation")
    set(TRAILBOOK_EV_EXPLANATION_MODULES_DIRECTORY "${TRAILBOOK_EV_EXPLANATION_DIRECTORY}/modules")

    _trailbook_ev_add_module_explanation_copy_explanation_command()

    add_custom_target(
        trailbook_${args_TRAILBOOK_NAME}_explanation_module_${args_MODULE_NAME}
        DEPENDS            
            ${MODULE_EXPLANATION_TARGET_FILES}
        COMMENT
            "Explanation module ${args_MODULE_NAME} for trailbook ${args_TRAILBOOK_NAME} is available."
    )
    set_property(
        TARGET
            trailbook_${args_TRAILBOOK_NAME}
        APPEND
        PROPERTY
            ADDITIONAL_DEPS_STAGE_BUILD_SPHINX_BEFORE
                ${MODULE_EXPLANATION_TARGET_FILES}
                trailbook_${args_TRAILBOOK_NAME}_explanation_module_${args_MODULE_NAME}
    )
endfunction()
