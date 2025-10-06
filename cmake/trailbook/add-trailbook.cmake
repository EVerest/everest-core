
macro(_check_requirements_txt)
    if(EXISTS ${args_REQUIREMENTS_TXT})
        execute_process(
            COMMAND
                ${Python3_EXECUTABLE}
                ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/check_requirements_txt.py
                ${args_REQUIREMENTS_TXT}
                --fix-in-venv
            RESULT_VARIABLE _CHECK_REQUIREMENTS_TXT_RESULT
            COMMAND_ERROR_IS_FATAL NONE
        )

        if(NOT ${_CHECK_REQUIREMENTS_TXT_RESULT} EQUAL 0)
            message(FATAL_ERROR "Trailbook: ${args_NAME} - ${args_REQUIREMENTS_TXT} not satisfied.")
        else()
            message(STATUS "Trailbook: ${args_NAME} - ${args_REQUIREMENTS_TXT} satisfied.")
        endif()
    else()
        message(STATUS "Trailbook: ${args_NAME} - No requirements.txt found.")
    endif()
endmacro()

# Internal macro to add a target that copies the stem files to the build directory.
# This macro is called by add_trailbook().
# It should not be called directly.
macro(_add_copy_stem_command)
    file(
        GLOB_RECURSE
        STEM_FILES_ORIGIN
        CONFIGURE_DEPENDS
        "${args_STEM_DIRECTORY}/*"
    )

    # predict output files based on input files
    set(STEM_FILES_BUILD "")
    foreach(file_path IN LISTS STEM_FILES_ORIGIN)
        file(RELATIVE_PATH rel_path "${args_STEM_DIRECTORY}" "${file_path}")
        list(APPEND STEM_FILES_BUILD "${args_SOURCE_DIRECTORY}/${rel_path}")
    endforeach()

    add_custom_command(
        OUTPUT
            ${STEM_FILES_BUILD}
        DEPENDS
            ${STEM_FILES_ORIGIN}
        COMMENT
            "Trailbook: ${args_NAME} - Copying stem files to build directory"
        COMMAND
            ${CMAKE_COMMAND} -E rm -rf ${args_SOURCE_DIRECTORY}
        COMMAND
            ${CMAKE_COMMAND} -E copy_directory ${args_STEM_DIRECTORY} ${args_SOURCE_DIRECTORY}
    )
    # add_dependencies(trailbook_${args_NAME} trailbook_${args_NAME}_stem)
endmacro()

macro(_add_build_html_target)
    set(CHECK_DONE_FILE "${CMAKE_CURRENT_BINARY_DIR}/build_html.check_done")

    add_custom_command(
        OUTPUT
            ${CHECK_DONE_FILE}
        DEPENDS
            ${STEM_FILES_BUILD}
        COMMENT
            "Trailbook: ${args_NAME} - Building HTML documentation with Sphinx"
        COMMAND
            ${CMAKE_COMMAND} -E rm -rf ${args_BUILD_HTML_DIRECTORY}
        COMMAND
            ${_SPHINX_BUILD_EXECUTABLE}
            -b html
            ${args_SOURCE_DIRECTORY}
            ${args_BUILD_HTML_DIRECTORY}
        COMMAND
            ${CMAKE_COMMAND} -E echo 
            "Trailbook: ${args_NAME} - HTML documentation built at ${args_BUILD_HTML_DIRECTORY}"
        COMMAND
            ${CMAKE_COMMAND} -E touch ${CHECK_DONE_FILE}
    )

    add_custom_target(
        trailbook_${args_NAME}_build_html
        DEPENDS
            ${CHECK_DONE_FILE}
        COMMENT
            "Trailbook: ${args_NAME} - Build HTML documentation"
    )
    add_dependencies(trailbook_${args_NAME} trailbook_${args_NAME}_build_html)
endmacro()

macro(_add_html_server_target)
    add_custom_target(
        trailbook_${args_NAME}_html_server
        DEPENDS
            trailbook_${args_NAME}_build_html
        COMMENT
            "Trailbook: ${args_NAME} - Serve HTML documentation"
        COMMAND
            ${CMAKE_COMMAND} -E echo 
            "Trailbook: ${args_NAME} - Serving HTML output at http://localhost:8000/"
        COMMAND
            ${Python3_EXECUTABLE} -m http.server --directory ${args_BUILD_HTML_DIRECTORY} 8000
        USES_TERMINAL
    )
endmacro()

macro(_add_auto_build_html_server_target)
    add_custom_target(
        trailbook_${args_NAME}_auto_build_html_server
        DEPENDS
            trailbook_${args_NAME}_build_html
        COMMENT
            "Trailbook: ${args_NAME} - Auto-build HTML documentation with Sphinx and serve"
        COMMAND
            ${CMAKE_COMMAND} -E echo 
            "Trailbook: ${args_NAME} - Auto-building HTML output and serving at http://localhost:8000/"
        COMMAND
            ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/target_observer.py
            "trailbook_${args_NAME}_build_html"
            "trailbook_${args_NAME}_html_server"
            --build-dir ${CMAKE_BINARY_DIR}
            --interval-ms 2000
        USES_TERMINAL
    )
endmacro()

# Add a new trailbook to the build.
# Parameters:
#   NAME (required): Name of the trailbook. This will be used to name the
#       build target and the output directory.
#   STEM_DIRECTORY (optional): Absolute path to the directory containing the
#       stem source files for the trailbook. Defaults to CMAKE_CURRENT_SOURCE_DIR.
#       This directory must exist and be an absolute path.
#
function(add_trailbook)
    set(options)
    set(one_value_args
        NAME
        STEM_DIRECTORY
        REQUIREMENTS_TXT
    )
    set(multi_value_args)

    cmake_parse_arguments(
        "args"
        "${options}"
        "${one_value_args}"
        "${multi_value_args}"
        ${ARGN}
    )

    # Parameter NAME
    #   is required
    if("${args_NAME}" STREQUAL "")
        message(FATAL_ERROR "add_trailbook: NAME argument is required")
    endif()

    # Parameter STEM_DIRECTORY
    #   - defaults to CMAKE_CURRENT_SOURCE_DIR
    #   - needs to be absolute path
    if("${args_STEM_DIRECTORY}" STREQUAL "")
        set(args_STEM_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()
    if(NOT IS_ABSOLUTE "${args_STEM_DIRECTORY}")
        message(FATAL_ERROR "add_trailbook: STEM_DIRECTORY needs to be an absolute path")
    endif()
    cmake_path(SET args_STEM_DIRECTORY NORMALIZE ${args_STEM_DIRECTORY})

    # Parameter REQUIREMENTS_TXT
    #   - defaults to ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt if exists
    #   - needs to be absolute path if set
    if("${args_REQUIREMENTS_TXT}" STREQUAL "")
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt")
            set(args_REQUIREMENTS_TXT "${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt")
        endif()
    endif()
    if(NOT "${args_REQUIREMENTS_TXT}" STREQUAL "")
        if(NOT IS_ABSOLUTE "${args_REQUIREMENTS_TXT}")
            message(FATAL_ERROR "add_trailbook: REQUIREMENTS_TXT needs to be an absolute path")
        endif()
        if(NOT EXISTS "${args_REQUIREMENTS_TXT}")
            message(FATAL_ERROR "add_trailbook: REQUIREMENTS_TXT file does not exist: ${args_REQUIREMENTS_TXT}")
        endif()
    endif()

    set(args_SOURCE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/trailbook_${args_NAME}_source")
    set(args_BUILD_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/trailbook_${args_NAME}_build")
    set(args_BUILD_HTML_DIRECTORY "${args_BUILD_DIRECTORY}/html")

    _check_requirements_txt()

    message (STATUS "Adding trailbook: ${args_NAME}")
    message (STATUS "  Stem directory: ${args_STEM_DIRECTORY}")

    add_custom_target(
        trailbook_${args_NAME}
        COMMENT
            "Build trailbook: ${args_NAME}"
    )
    set_target_properties(
        trailbook_${args_NAME}
        PROPERTIES
            SOURCE_DIRECTORY "${args_SOURCE_DIRECTORY}"
    )

    _add_copy_stem_command()
    _add_build_html_target()
    _add_html_server_target()
    _add_auto_build_html_server_target()
endfunction()
