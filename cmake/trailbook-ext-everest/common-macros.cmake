include_guard(GLOBAL)

macro(_trailbook_ev_check_reference_dir_command)
    get_target_property(
        _EXT_EV_CHECK_REFERENCE_DIR_COMMAND_ADDED
        trailbook_${args_TRAILBOOK_NAME}
        _EXT_EV_CHECK_REFERENCE_DIR_COMMAND_ADDED
    )
    set(CHECK_DONE_FILE_CHECK_REFERENCE_DIR "${TRAILBOOK_CURRENT_BINARY_DIR}/ext-ev.check-reference-dir.check_done")
    if("${_EXT_EV_CHECK_REFERENCE_DIR_COMMAND_ADDED}" STREQUAL "_EXT_EV_CHECK_REFERENCE_DIR_COMMAND_ADDED-NOTFOUND")
        get_target_property(
            DEPS_STAGE_PREPARE_SPHINX_SOURCE_AFTER
            trailbook_${args_TRAILBOOK_NAME}
            DEPS_STAGE_PREPARE_SPHINX_SOURCE_AFTER
        )
        add_custom_command(
            OUTPUT
                ${CHECK_DONE_FILE_CHECK_REFERENCE_DIR}
            DEPENDS
                ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/check_dir_exists.py
                trailbook_${args_TRAILBOOK_NAME}_stage_prepare_sphinx_source_after
                ${DEPS_STAGE_PREPARE_SPHINX_SOURCE_AFTER}
            COMMENT
                "Checking existence of reference directory: ${TRAILBOOK_EV_REFERENCE_DIRECTORY}"
            COMMAND
                ${Python3_EXECUTABLE}
                ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/check_dir_exists.py
                --directory "${TRAILBOOK_EV_REFERENCE_DIRECTORY}"
                --return-zero-if-exists
            COMMAND
                ${CMAKE_COMMAND} -E touch ${CHECK_DONE_FILE_CHECK_REFERENCE_DIR}
        )
        set_target_properties(
            trailbook_${args_TRAILBOOK_NAME}
            PROPERTIES
                _EXT_EV_CHECK_REFERENCE_DIR_COMMAND_ADDED TRUE
        )
        set_property(
            TARGET
                trailbook_${args_TRAILBOOK_NAME}
            APPEND
            PROPERTY
                ADDITIONAL_DEPS_STAGE_BUILD_SPHINX_BEFORE ${CHECK_DONE_FILE_CHECK_REFERENCE_DIR}
        )
    endif()
endmacro()
