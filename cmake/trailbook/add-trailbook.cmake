
macro(_add_trailbook_check_requirements_txt)
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

macro(_add_trailbook_download_all_versions_command)
    find_program(
        GIT_EXECUTABLE
        NAMES git
        REQUIRED
    )

    add_custom_command(
        OUTPUT
            ${CHECK_DONE_FILE_DOWNLOAD_ALL_VERSIONS}
        DEPENDS            
            trailbook_${args_NAME}_stage_prepare_sphinx_source_before
        COMMENT
            "Trailbook: ${args_NAME} - Downloading all versions repo"
        COMMAND
            ${CMAKE_COMMAND} -E rm -rf
            ${CMAKE_CURRENT_BINARY_DIR}/deployed_docs_repo/
            ${TRAILBOOK_BUILD_DIRECTORY}
            ${CHECK_DONE_FILE_CREATE_EMPTY_SKELETON_MULTIVERSION_ROOT}
        COMMAND
            ${GIT_EXECUTABLE} clone 
            -b ${args_DEPLOYED_DOCS_REPO_BRANCH} 
            --depth 1
            ${args_DEPLOYED_DOCS_REPO_URL}
            ${CMAKE_CURRENT_BINARY_DIR}/deployed_docs_repo/
        COMMAND
            ${CMAKE_COMMAND} -E create_symlink
            ${CMAKE_CURRENT_BINARY_DIR}/deployed_docs_repo/docs/
            ${TRAILBOOK_BUILD_DIRECTORY}
        COMMAND
            ${CMAKE_COMMAND} -E touch ${CHECK_DONE_FILE_DOWNLOAD_ALL_VERSIONS}
    )
    # Reset variable, so that other commands do not depend on it
    # if this command is not used
    set(CHECK_DONE_FILE_CREATE_EMPTY_SKELETON_MULTIVERSION_ROOT "")
endmacro()

macro(_add_trailbook_create_empty_skeleton_multiversion_root_command)
    add_custom_command(
        OUTPUT
            ${CHECK_DONE_FILE_CREATE_EMPTY_SKELETON_MULTIVERSION_ROOT}
        DEPENDS
            trailbook_${args_NAME}_stage_prepare_sphinx_source_before
        COMMENT
            "Trailbook: ${args_NAME} - Creating empty skeleton multiversion root directory"
        COMMAND
            ${CMAKE_COMMAND} -E rm -rf
            ${TRAILBOOK_BUILD_DIRECTORY}
            ${CHECK_DONE_FILE_DOWNLOAD_ALL_VERSIONS}
        COMMAND
            ${CMAKE_COMMAND} -E make_directory
            ${TRAILBOOK_BUILD_DIRECTORY}/
        COMMAND
            ${CMAKE_COMMAND} -E touch ${CHECK_DONE_FILE_CREATE_EMPTY_SKELETON_MULTIVERSION_ROOT}
    )
    # Reset variable, so that other commands do not depend on it
    # if this command is not used
    set(CHECK_DONE_FILE_DOWNLOAD_ALL_VERSIONS "")
endmacro()

macro(_add_trailbook_copy_stem_command)
    file(
        GLOB_RECURSE
        STEM_FILES_SOURCE_DIR
        CONFIGURE_DEPENDS
        "${args_STEM_DIRECTORY}/*"
    )

    set(STEM_FILES_BUILD_DIR "")
    foreach(file_path IN LISTS STEM_FILES_SOURCE_DIR)
        file(RELATIVE_PATH rel_path "${args_STEM_DIRECTORY}" "${file_path}")
        list(APPEND STEM_FILES_BUILD_DIR "${TRAILBOOK_INSTANCE_SOURCE_DIRECTORY}/${rel_path}")
    endforeach()

    add_custom_command(
        OUTPUT
            ${STEM_FILES_BUILD_DIR}
        DEPENDS
            ${STEM_FILES_SOURCE_DIR}
            ${CHECK_DONE_FILE_DOWNLOAD_ALL_VERSIONS}
            ${CHECK_DONE_FILE_CREATE_EMPTY_SKELETON_MULTIVERSION_ROOT}
            trailbook_${args_NAME}_stage_prepare_sphinx_source_before
        COMMENT
            "Trailbook: ${args_NAME} - Copying stem files to build directory"
        COMMAND
            ${CMAKE_COMMAND} -E rm -rf ${TRAILBOOK_INSTANCE_SOURCE_DIRECTORY}
        COMMAND
            ${CMAKE_COMMAND} -E copy_directory ${args_STEM_DIRECTORY} ${TRAILBOOK_INSTANCE_SOURCE_DIRECTORY}
    )
endmacro()

macro(_add_trailbook_create_metadata_yaml_command)
    set(METADATA_YAML_FILE "${CMAKE_CURRENT_BINARY_DIR}/metadata_${args_NAME}.yaml")

    add_custom_command(
        OUTPUT
            ${METADATA_YAML_FILE}
        DEPENDS
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/create_metadata_yaml.py
            ${STEM_FILES_BUILD_DIR}
            ${CHECK_DONE_FILE_DOWNLOAD_ALL_VERSIONS}
            ${CHECK_DONE_FILE_CREATE_EMPTY_SKELETON_MULTIVERSION_ROOT}
            trailbook_${args_NAME}_stage_prepare_sphinx_source_before
        COMMENT
            "Trailbook: ${args_NAME} - Creating metadata YAML file"
        COMMAND
            ${CMAKE_COMMAND} -E rm -f ${METADATA_YAML_FILE}
        COMMAND
            ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/create_metadata_yaml.py
            --multiversion-root-directory "${TRAILBOOK_BUILD_DIRECTORY}"
            "--output-path" "${METADATA_YAML_FILE}"
            --additional-version "${args_INSTANCE_NAME}"
    )
endmacro()

macro(_add_trailbook_sphinx_build_command)
    set(CHECK_DONE_FILE_SPHINX_BUILD_COMMAND "${CMAKE_CURRENT_BINARY_DIR}/build_html.check_done")

    add_custom_command(
        OUTPUT
            ${CHECK_DONE_FILE_SPHINX_BUILD_COMMAND}
        DEPENDS
            trailbook_${args_NAME}_stage_build_sphinx_before
            ${STEM_FILES_BUILD_DIR}
            ${METADATA_YAML_FILE}
        COMMENT
            "Trailbook: ${args_NAME} - Building HTML documentation with Sphinx"
        COMMAND
            ${CMAKE_COMMAND} -E rm -rf
            ${TRAILBOOK_INSTANCE_BUILD_DIRECTORY}/
        COMMAND
            EVEREST_METADATA_YAML_PATH=${METADATA_YAML_FILE}
            ${_SPHINX_BUILD_EXECUTABLE}
            -b html
            ${TRAILBOOK_INSTANCE_SOURCE_DIRECTORY}
            ${TRAILBOOK_INSTANCE_BUILD_DIRECTORY}
        COMMAND
            ${CMAKE_COMMAND} -E echo 
            "Trailbook: ${args_NAME} - HTML documentation built at ${TRAILBOOK_INSTANCE_BUILD_DIRECTORY}"
        COMMAND
            ${CMAKE_COMMAND} -E touch ${CHECK_DONE_FILE_SPHINX_BUILD_COMMAND}
    )
endmacro()

macro(_add_trailbook_replace_latest_command)
    set(CHECK_DONE_FILE_REPLACE_LATEST "${CMAKE_CURRENT_BINARY_DIR}/replace_latest.check_done")
    add_custom_command(
        OUTPUT
            ${CHECK_DONE_FILE_REPLACE_LATEST}
        DEPENDS
            trailbook_${args_NAME}_stage_postprocess_sphinx_before
            ${CHECK_DONE_FILE_SPHINX_BUILD_COMMAND}
        COMMENT
            "Trailbook: ${args_NAME} - Replacing 'latest' copy with copy of current instance"
        COMMAND
            ${CMAKE_COMMAND} -E rm -rf ${TRAILBOOK_BUILD_DIRECTORY}/latest
        COMMAND
            ${CMAKE_COMMAND} -E copy_directory
            ${TRAILBOOK_INSTANCE_BUILD_DIRECTORY}
            ${TRAILBOOK_BUILD_DIRECTORY}/latest
        COMMAND
            ${CMAKE_COMMAND} -E touch ${CHECK_DONE_FILE_REPLACE_LATEST}
    )
endmacro()

macro(_add_trailbook_copy_404_command)
    set(CHECK_DONE_FILE_COPY_404 "${CMAKE_CURRENT_BINARY_DIR}/copy_404.check_done")
    set(TRAILBOOK_404_FILE "${TRAILBOOK_BUILD_DIRECTORY}/404.html")
    set(TRAILBOOK_INSTANCE_404_FILE "${TRAILBOOK_INSTANCE_BUILD_DIRECTORY}/404.html")
    add_custom_command(
        OUTPUT
            ${TRAILBOOK_INSTANCE_404_FILE}
        DEPENDS
            trailbook_${args_NAME}_stage_postprocess_sphinx_before
            ${CHECK_DONE_FILE_SPHINX_BUILD_COMMAND}
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/check_404_file.py
        COMMENT
            "Trailbook: ${args_NAME} - Checking for 404.html in built documentation"
        COMMAND
            ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/check_404_file.py
            --404-file "${TRAILBOOK_INSTANCE_404_FILE}"
    )    
    add_custom_command(
        OUTPUT
            ${CHECK_DONE_FILE_COPY_404}
        DEPENDS
            trailbook_${args_NAME}_stage_postprocess_sphinx_before
            ${CHECK_DONE_FILE_SPHINX_BUILD_COMMAND}
            ${TRAILBOOK_INSTANCE_404_FILE}
        COMMENT
            "Trailbook: ${args_NAME} - Copying 404.html to multiversion root directory"
        COMMAND
            ${CMAKE_COMMAND} -E rm -f ${TRAILBOOK_404_FILE}
        COMMAND
            ${CMAKE_COMMAND} -E copy
            ${TRAILBOOK_INSTANCE_404_FILE}
            ${TRAILBOOK_404_FILE}
        COMMAND
            ${CMAKE_COMMAND} -E touch ${CHECK_DONE_FILE_COPY_404}
    )
endmacro()

macro(_add_trailbook_render_redirect_template_command)
    set(CHECK_DONE_FILE_RENDER_REDIRECT_TEMPLATE "${CMAKE_CURRENT_BINARY_DIR}/render_redirect_template.check_done")
    set(REDIRECT_TEMPLATE_FILE "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/templates/redirect.html.jinja")
    set(TRAILBOOK_REDIRECT_FILE "${TRAILBOOK_BUILD_DIRECTORY}/index.html")
    add_custom_command(
        OUTPUT
            ${CHECK_DONE_FILE_RENDER_REDIRECT_TEMPLATE}
        DEPENDS
            trailbook_${args_NAME}_stage_postprocess_sphinx_before
            ${CHECK_DONE_FILE_SPHINX_BUILD_COMMAND}
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/render_redirect_template.py
        COMMENT
            "Trailbook: ${args_NAME} - Rendering redirect.html from template"
        COMMAND
            ${CMAKE_COMMAND} -E rm -f ${TRAILBOOK_REDIRECT_FILE}
        COMMAND
            ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/render_redirect_template.py
            --redirect-template "${REDIRECT_TEMPLATE_FILE}"
            "--target-path" "${TRAILBOOK_REDIRECT_FILE}"
        COMMAND
            ${CMAKE_COMMAND} -E touch ${CHECK_DONE_FILE_RENDER_REDIRECT_TEMPLATE}
    )
endmacro()

macro(_add_trailbook_copy_versions_index_command)
    set(CHECK_DONE_FILE_COPY_VERSIONS_INDEX "${CMAKE_CURRENT_BINARY_DIR}/copy_versions_index.check_done")
    set(TRAILBOOK_VERSIONS_INDEX_FILE "${TRAILBOOK_BUILD_DIRECTORY}/versions_index.html")
    set(TRAILBOOK_INSTANCE_VERSIONS_INDEX_FILE "${TRAILBOOK_INSTANCE_BUILD_DIRECTORY}/versions_index.html")
    set(CHECK_DONE_FILE_CHECK_LATEST_INSTANCE "${CMAKE_CURRENT_BINARY_DIR}/check_latest_instance.check_done")
    add_custom_command(
        OUTPUT
            ${TRAILBOOK_INSTANCE_VERSIONS_INDEX_FILE}
        DEPENDS
            trailbook_${args_NAME}_stage_postprocess_sphinx_before
            ${CHECK_DONE_FILE_SPHINX_BUILD_COMMAND}
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/check_versions_index_file.py
        COMMENT
            "Trailbook: ${args_NAME} - Checking for versions_index.html in built documentation"
        COMMAND
            ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/check_versions_index_file.py
            --versions-index-file "${TRAILBOOK_INSTANCE_VERSIONS_INDEX_FILE}"
    )
    add_custom_command(
        OUTPUT
            ${CHECK_DONE_FILE_CHECK_LATEST_INSTANCE}
        DEPENDS
            trailbook_${args_NAME}_stage_postprocess_sphinx_before
            ${CHECK_DONE_FILE_SPHINX_BUILD_COMMAND}
            ${CHECK_DONE_FILE_REPLACE_LATEST}
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/check_latest_instance.py
        COMMENT
            "Trailbook: ${args_NAME} - Checking for latest/ in multiversion root directory"
        COMMAND
            ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/check_latest_instance.py
            --multiversion-root-directory "${TRAILBOOK_BUILD_DIRECTORY}"
        COMMAND
            ${CMAKE_COMMAND} -E touch ${CHECK_DONE_FILE_CHECK_LATEST_INSTANCE}
    )
    add_custom_command(
        OUTPUT
            ${CHECK_DONE_FILE_COPY_VERSIONS_INDEX}
        DEPENDS
            trailbook_${args_NAME}_stage_postprocess_sphinx_before
            ${CHECK_DONE_FILE_SPHINX_BUILD_COMMAND}
            ${TRAILBOOK_INSTANCE_VERSIONS_INDEX_FILE}
            ${CHECK_DONE_FILE_CHECK_LATEST_INSTANCE}
        COMMENT
            "Trailbook: ${args_NAME} - Copying versions_index.html to multiversion root directory"
        COMMAND
            ${CMAKE_COMMAND} -E rm -f ${TRAILBOOK_VERSIONS_INDEX_FILE}
        COMMAND
            ${CMAKE_COMMAND} -E copy
            ${TRAILBOOK_INSTANCE_VERSIONS_INDEX_FILE}
            ${TRAILBOOK_VERSIONS_INDEX_FILE}
        COMMAND
            ${CMAKE_COMMAND} -E touch ${CHECK_DONE_FILE_COPY_VERSIONS_INDEX}
    )
endmacro()

macro(_add_trailbook_preview_target)
    add_custom_target(
        trailbook_${args_NAME}_preview
        DEPENDS
            trailbook_${args_NAME}
        COMMENT
            "Trailbook: ${args_NAME} - Serve HTML documentation"
        COMMAND
            ${CMAKE_COMMAND} -E echo 
            "Trailbook: ${args_NAME} - Serving HTML output at http://localhost:8000/"
        COMMAND
            ${Python3_EXECUTABLE} -m http.server --directory ${TRAILBOOK_BUILD_DIRECTORY} 8000
        USES_TERMINAL
    )
endmacro()

macro(_add_trailbook_live_preview_target)
    add_custom_target(
        trailbook_${args_NAME}_live_preview
        DEPENDS
            trailbook_${args_NAME}
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/target_observer.py
        COMMENT
            "Trailbook: ${args_NAME} - Auto-build HTML documentation with Sphinx and serve"
        COMMAND
            ${CMAKE_COMMAND} -E echo 
            "Trailbook: ${args_NAME} - Auto-building HTML output and serving at http://localhost:8000/"
        COMMAND
            ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/target_observer.py
            "trailbook_${args_NAME}"
            "trailbook_${args_NAME}_preview"
            --build-dir ${CMAKE_BINARY_DIR}
            --interval-ms 2000
        USES_TERMINAL
    )
endmacro()

function(add_trailbook)
    set(options)
    set(one_value_args
        NAME
        STEM_DIRECTORY
        REQUIREMENTS_TXT
        INSTANCE_NAME
        DEPLOYED_DOCS_REPO_URL
        DEPLOYED_DOCS_REPO_BRANCH
    )
    set(multi_value_args)
    cmake_parse_arguments(
        "args"
        "${options}"
        "${one_value_args}"
        "${multi_value_args}"
        ${ARGN}
    )

    option(TRAILBOOK_${args_NAME}_DOWNLOAD_ALL_VERSIONS "Download all versions for trailbook ${args_NAME} and build complete trailbook" OFF)
    option(TRAILBOOK_${args_NAME}_IS_RELEASE "If enabled, the trailbook ${args_NAME} will be marked as release version in versions index" ON)
    if(NOT TRAILBOOK_${args_NAME}_DOWNLOAD_ALL_VERSIONS AND NOT TRAILBOOK_${args_NAME}_IS_RELEASE)
        message(FATAL_ERROR "add_trailbook: TRAILBOOK_${args_NAME}_DOWNLOAD_ALL_VERSIONS and TRAILBOOK_${args_NAME}_IS_RELEASE cannot both be OFF")
    endif()


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

    # Parameter INSTANCE_NAME
    #   - required
    #   - needs to be lowercase alphanumeric and underscores only
    if("${args_INSTANCE_NAME}" STREQUAL "")
        message(FATAL_ERROR "add_trailbook: INSTANCE_NAME argument is required")
    endif()
    string(REGEX MATCH "^[a-z0-9_]+$" _valid_instance_name "${args_INSTANCE_NAME}")
    if("${_valid_instance_name}" STREQUAL "")
        message(FATAL_ERROR "add_trailbook: INSTANCE_NAME needs to be lowercase alphanumeric and underscores only")
    endif()

    # Parameter DEPLOYED_DOCS_REPO_URL
    #    - required if TRAILBOOK_<NAME>_DOWNLOAD_ALL_VERSIONS is ON
    if(TRAILBOOK_${args_NAME}_DOWNLOAD_ALL_VERSIONS AND "${args_DEPLOYED_DOCS_REPO_URL}" STREQUAL "")
        message(FATAL_ERROR "add_trailbook: DEPLOYED_DOCS_REPO_URL argument is required if TRAILBOOK_${args_NAME}_DOWNLOAD_ALL_VERSIONS is ON")
    endif()

    # Parameter DEPLOYED_DOCS_REPO_BRANCH
    #    - defaults to 'main'
    if("${args_DEPLOYED_DOCS_REPO_BRANCH}" STREQUAL "")
        set(args_DEPLOYED_DOCS_REPO_BRANCH "main")
    endif()

    set(TRAILBOOK_INSTANCE_SOURCE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/trailbook_${args_NAME}_source")
    set(TRAILBOOK_BUILD_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/trailbook_${args_NAME}_build")
    set(TRAILBOOK_INSTANCE_BUILD_DIRECTORY "${TRAILBOOK_BUILD_DIRECTORY}/${args_INSTANCE_NAME}")
    set(TRAILBOOK_INSTANCE_IS_RELEASE "${TRAILBOOK_${args_NAME}_IS_RELEASE}")
    set(TRAILBOOK_INSTANCE_DOWNLOAD_ALL_VERSIONS "${TRAILBOOK_${args_NAME}_DOWNLOAD_ALL_VERSIONS}")

    message(STATUS "Adding trailbook:               ${args_NAME}")
    message(STATUS "  Stem directory:               ${args_STEM_DIRECTORY}")
    message(STATUS "  Build directory:              ${TRAILBOOK_BUILD_DIRECTORY}")
    message(STATUS "  Instance source directory:    ${TRAILBOOK_INSTANCE_SOURCE_DIRECTORY}")
    message(STATUS "  Instance build directory:     ${TRAILBOOK_INSTANCE_BUILD_DIRECTORY}")
    if(NOT "${args_REQUIREMENTS_TXT}" STREQUAL "")
        message(STATUS "  Requirements.txt:             ${args_REQUIREMENTS_TXT}")
    else()
        message(STATUS "  Requirements.txt:             <none>")
    endif()
    message(STATUS "  Deployed docs repo url:       ${args_DEPLOYED_DOCS_REPO_URL}")
    message(STATUS "  Deployed docs repo branch:    ${args_DEPLOYED_DOCS_REPO_BRANCH}")


    add_custom_target(
        trailbook_${args_NAME}_stage_prepare_sphinx_source_before
    )
    _add_trailbook_check_requirements_txt()

    set(CHECK_DONE_FILE_DOWNLOAD_ALL_VERSIONS "${CMAKE_CURRENT_BINARY_DIR}/download_all_versions.check_done")
    set(CHECK_DONE_FILE_CREATE_EMPTY_SKELETON_MULTIVERSION_ROOT "${CMAKE_CURRENT_BINARY_DIR}/create_empty_skeleton_multiversion_root.check_done")
    if(TRAILBOOK_INSTANCE_DOWNLOAD_ALL_VERSIONS)
        _add_trailbook_download_all_versions_command()
    else()
        _add_trailbook_create_empty_skeleton_multiversion_root_command()
    endif()
    _add_trailbook_copy_stem_command()
    _add_trailbook_create_metadata_yaml_command()
    add_custom_target(
        trailbook_${args_NAME}_stage_prepare_sphinx_source_after
        DEPENDS
            trailbook_${args_NAME}_stage_prepare_sphinx_source_before
            ${CHECK_DONE_FILE_DOWNLOAD_ALL_VERSIONS}
            ${CHECK_DONE_FILE_CREATE_EMPTY_SKELETON_MULTIVERSION_ROOT}
            ${STEM_FILES_BUILD_DIR}
            ${METADATA_YAML_FILE}
        COMMENT
            "Prepare Sphinx source for trailbook: ${args_NAME}"
    )
    add_custom_target(
        trailbook_${args_NAME}_stage_build_sphinx_before
        DEPENDS
            trailbook_${args_NAME}_stage_prepare_sphinx_source_after
    )
    _add_trailbook_sphinx_build_command()
    add_custom_target(
        trailbook_${args_NAME}_stage_build_sphinx_after
        DEPENDS
            trailbook_${args_NAME}_stage_build_sphinx_before
            ${CHECK_DONE_FILE_SPHINX_BUILD_COMMAND}
        COMMENT
            "Build Sphinx documentation for trailbook: ${args_NAME}"
    )
    add_custom_target(
        trailbook_${args_NAME}_stage_postprocess_sphinx_before
        DEPENDS
            trailbook_${args_NAME}_stage_build_sphinx_after
    )
    if(TRAILBOOK_INSTANCE_IS_RELEASE)
        _add_trailbook_replace_latest_command()
        _add_trailbook_copy_404_command()
        _add_trailbook_render_redirect_template_command()
    endif()
    _add_trailbook_copy_versions_index_command()

    add_custom_target(
        trailbook_${args_NAME}_stage_postprocess_sphinx_after
        DEPENDS
        trailbook_${args_NAME}_stage_postprocess_sphinx_before
            ${CHECK_DONE_FILE_REPLACE_LATEST}
            ${CHECK_DONE_FILE_COPY_404}
            ${CHECK_DONE_FILE_COPY_VERSIONS_INDEX}
            ${CHECK_DONE_FILE_RENDER_REDIRECT_TEMPLATE}
        COMMENT
            "Post-process Sphinx documentation for trailbook: ${args_NAME}"
    )
    add_custom_target(
        trailbook_${args_NAME} ALL
        DEPENDS
            trailbook_${args_NAME}_stage_postprocess_sphinx_after
        COMMENT
            "Build trailbook: ${args_NAME}"
    )

    _add_trailbook_preview_target()
    _add_trailbook_live_preview_target()

    set_target_properties(
        trailbook_${args_NAME}
        PROPERTIES
            TRAILBOOK_INSTANCE_BUILD_DIRECTORY "${TRAILBOOK_INSTANCE_BUILD_DIRECTORY}"
            TRAILBOOK_BUILD_DIRECTORY "${TRAILBOOK_BUILD_DIRECTORY}"
            TRAILBOOK_INSTANCE_NAME "${args_INSTANCE_NAME}"
            TRAILBOOK_INSTANCE_SOURCE_DIRECTORY "${TRAILBOOK_INSTANCE_SOURCE_DIRECTORY}"
            TRAILBOOK_CURRENT_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}"
    )
endfunction()
