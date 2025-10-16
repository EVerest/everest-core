macro(_trailbook_ev_add_module_explanation_check_explanation_dir_command)
    get_target_property(
        _EXT_EV_CHECK_EXPLANATION_DIR_COMMAND_ADDED
        trailbook_${args_TRAILBOOK_NAME}
        _EXT_EV_CHECK_EXPLANATION_DIR_COMMAND_ADDED
    )
    set(CHECK_DONE_FILE_CHECK_EXPLANATION_DIR "${TRAILBOOK_CURRENT_BINARY_DIR}/ext-ev.check-explanation-dir.check_done")
    if("${_EXT_EV_CHECK_EXPLANATION_DIR_COMMAND_ADDED}" STREQUAL "_EXT_EV_CHECK_EXPLANATION_DIR_COMMAND_ADDED-NOTFOUND")
        add_custom_command(
            OUTPUT
                ${CHECK_DONE_FILE_CHECK_EXPLANATION_DIR}
            DEPENDS
                ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/check_dir_exists.py
                trailbook_${args_TRAILBOOK_NAME}_stage_prepare_sphinx_source_after
            COMMENT
                "Checking existence of explanation directory: ${TRAILBOOK_EV_EXPLANATION_DIRECTORY}"
            COMMAND
                ${Python3_EXECUTABLE}
                ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/check_dir_exists.py
                --directory "${TRAILBOOK_EV_EXPLANATION_DIRECTORY}"
                --return-zero-if-exists
            COMMAND
                ${CMAKE_COMMAND} -E touch ${CHECK_DONE_FILE_CHECK_EXPLANATION_DIR}
        )
        set_target_properties(
            trailbook_${args_TRAILBOOK_NAME}
            PROPERTIES
                _EXT_EV_CHECK_EXPLANATION_DIR_COMMAND_ADDED TRUE
        )
    endif()
endmacro()

macro(_trailbook_ev_add_module_explanation_check_explanation_modules_dir_command)
    get_target_property(
        _EXT_EV_CHECK_EXPLANATION_MODULES_DIR_COMMAND_ADDED
        trailbook_${args_TRAILBOOK_NAME}
        _EXT_EV_CHECK_EXPLANATION_MODULES_DIR_COMMAND_ADDED
    )
    set(CHECK_DONE_FILE_CHECK_EXPLANATION_MODULES_DIR "${TRAILBOOK_CURRENT_BINARY_DIR}/ext-ev.check-explanation-modules-dir.check_done")
    if("${_EXT_EV_CHECK_EXPLANATION_MODULES_DIR_COMMAND_ADDED}" STREQUAL "_EXT_EV_CHECK_EXPLANATION_MODULES_DIR_COMMAND_ADDED-NOTFOUND")
        add_custom_command(
            OUTPUT
                ${CHECK_DONE_FILE_CHECK_EXPLANATION_MODULES_DIR}
            DEPENDS
                ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/check_dir_exists.py
                trailbook_${args_TRAILBOOK_NAME}_stage_prepare_sphinx_source_after
                ${CHECK_DONE_FILE_CHECK_EXPLANATION_DIR}
            COMMENT
                "Checking existence of explanation modules directory: ${TRAILBOOK_EV_EXPLANATION_MODULES_DIRECTORY}"
            COMMAND
                ${Python3_EXECUTABLE}
                ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/check_dir_exists.py
                --directory "${TRAILBOOK_EV_EXPLANATION_MODULES_DIRECTORY}"
                --return-zero-if-not-exists
            COMMAND
                ${CMAKE_COMMAND} -E touch ${CHECK_DONE_FILE_CHECK_EXPLANATION_MODULES_DIR}
        )
        set_target_properties(
            trailbook_${args_TRAILBOOK_NAME}
            PROPERTIES
                _EXT_EV_CHECK_EXPLANATION_MODULES_DIR_COMMAND_ADDED TRUE
        )
    endif()
endmacro()

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
            ${CHECK_DONE_FILE_CHECK_EXPLANATION_DIR}
            ${CHECK_DONE_FILE_CHECK_EXPLANATION_MODULES_DIR}
        COMMENT
            "Copying explanation module ${args_MODULE_NAME} files to: ${TRAILBOOK_EV_EXPLANATION_MODULES_DIRECTORY}/${args_MODULE_NAME}"
        COMMAND
            ${CMAKE_COMMAND} -E rm -rf "${TRAILBOOK_EV_EXPLANATION_MODULES_DIRECTORY}/${args_MODULE_NAME}"
        COMMAND
            ${CMAKE_COMMAND} -E make_directory "${TRAILBOOK_EV_EXPLANATION_MODULES_DIRECTORY}/${args_MODULE_NAME}"
        COMMAND
            ${CMAKE_COMMAND} -E copy_directory "${args_EXPLANATION_DIR}" "${TRAILBOOK_EV_EXPLANATION_MODULES_DIRECTORY}/${args_MODULE_NAME}"
    )
endmacro()

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
        TRAILBOOK_CURRENT_BINARY_DIR
        trailbook_${args_TRAILBOOK_NAME}
        TRAILBOOK_CURRENT_BINARY_DIR
    )

    set(TRAILBOOK_EV_EXPLANATION_DIRECTORY "${TRAILBOOK_INSTANCE_SOURCE_DIRECTORY}/explanation")
    set(TRAILBOOK_EV_EXPLANATION_MODULES_DIRECTORY "${TRAILBOOK_EV_EXPLANATION_DIRECTORY}/modules")


    _trailbook_ev_add_module_explanation_check_explanation_dir_command()
    _trailbook_ev_add_module_explanation_check_explanation_modules_dir_command()
    _trailbook_ev_add_module_explanation_copy_explanation_command()

    add_custom_target(
        trailbook_${args_TRAILBOOK_NAME}_explanation_module_${args_MODULE_NAME}
        DEPENDS            
            ${MODULE_EXPLANATION_TARGET_FILES}
        COMMENT
            "Explanation module ${args_MODULE_NAME} for trailbook ${args_TRAILBOOK_NAME} is available."
    )
    add_dependencies(
        trailbook_${args_TRAILBOOK_NAME}_stage_build_sphinx_before
        trailbook_${args_TRAILBOOK_NAME}_explanation_module_${args_MODULE_NAME}
    )
endfunction()
