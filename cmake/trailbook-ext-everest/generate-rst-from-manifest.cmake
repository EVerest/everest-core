include(${CMAKE_CURRENT_LIST_DIR}/common-macros.cmake)

macro(_trailbook_ev_generate_rst_from_manifest_check_reference_modules_dir_command)
    get_target_property(
        _EXT_EV_CHECK_REFERENCE_MODULES_DIR_COMMAND_ADDED
        trailbook_${args_TRAILBOOK_NAME}
        _EXT_EV_CHECK_REFERENCE_MODULES_DIR_COMMAND_ADDED
    )
    set(CHECK_DONE_FILE_CHECK_REFERENCE_MODULES_DIR "${TRAILBOOK_CURRENT_BINARY_DIR}/ext-ev.check-reference-modules-dir.check_done")
    if("${_EXT_EV_CHECK_REFERENCE_MODULES_DIR_COMMAND_ADDED}" STREQUAL "_EXT_EV_CHECK_REFERENCE_MODULES_DIR_COMMAND_ADDED-NOTFOUND")
        add_custom_command(
            OUTPUT
                ${CHECK_DONE_FILE_CHECK_REFERENCE_MODULES_DIR}
            DEPENDS
                ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/check_dir_exists.py
                trailbook_${args_TRAILBOOK_NAME}_stage_prepare_sphinx_source_after
                ${DEPS_STAGE_PREPARE_SPHINX_SOURCE_AFTER}
                ${CHECK_DONE_FILE_CHECK_REFERENCE_DIR}
            COMMENT
                "Checking existence of reference modules directory: ${TRAILBOOK_EV_REFERENCE_MODULES_DIRECTORY}"
            COMMAND
                ${Python3_EXECUTABLE}
                ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/check_dir_exists.py
                --directory "${TRAILBOOK_EV_REFERENCE_MODULES_DIRECTORY}"
                --return-zero-if-not-exists
            COMMAND
                ${CMAKE_COMMAND} -E touch ${CHECK_DONE_FILE_CHECK_REFERENCE_MODULES_DIR}
        )
        set_target_properties(
            trailbook_${args_TRAILBOOK_NAME}
            PROPERTIES
                _EXT_EV_CHECK_REFERENCE_MODULES_DIR_COMMAND_ADDED TRUE
        )
    endif()
endmacro()

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
            ${CHECK_DONE_FILE_CHECK_REFERENCE_DIR}
            ${CHECK_DONE_FILE_CHECK_REFERENCE_MODULES_DIR}
            ${TEMPLATES_DIRECTORY}/module.rst.jinja
            ${TEMPLATES_DIRECTORY}/macros.jinja
            trailbook_${args_TRAILBOOK_NAME}_stage_prepare_sphinx_source_after
            ${DEPS_STAGE_PREPARE_SPHINX_SOURCE_AFTER}
        COMMENT
            "Generating RST file ${GENERATED_FILE} from manifest definition ${args_MANIFEST_FILE}"
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
    set(TRAILBOOK_EV_REFERENCE_MODULES_DIRECTORY "${TRAILBOOK_EV_REFERENCE_DIRECTORY}/modules")
    get_filename_component(MODULE_DIR ${args_MANIFEST_FILE} DIRECTORY)
    get_filename_component(MODULE_NAME ${MODULE_DIR} NAME_WE)


    _trailbook_ev_check_reference_dir_command()
    _trailbook_ev_generate_rst_from_manifest_check_reference_modules_dir_command()
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
                ${CHECK_DONE_FILE_CHECK_REFERENCE_MODULES_DIR}
                ${GENERATED_FILE}
                trailbook_${args_TRAILBOOK_NAME}_generate_rst_from_manifest_${MODULE_NAME}
    )
endfunction()
