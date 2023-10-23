if (NOT EVEREST_SCHEMA_DIR)
    get_filename_component(CUR_FILE_NAME ${CMAKE_CURRENT_LIST_FILE} NAME)
    message(FATAL_ERROR "\
The variable EVEREST_SCHEMA_DIR is not set, this needs to be done, \
before including \"${CUR_FILE_NAME}\"\
    ")
endif()

set (EV_CORE_CMAKE_SCRIPT_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE FILEPATH "")

# FIXME (aw): where should this go, should it be global?
string(ASCII 27 ESCAPE)
set(FMT_RESET "${ESCAPE}[m")
set(FMT_BOLD "${ESCAPE}[1m")

message(STATUS "${COLOR_BOLD}Should be bold?${COLOR_RESET}")

add_custom_target(convert_yaml_refs)
set_target_properties(convert_yaml_refs
    PROPERTIES
        EVEREST_CONVERTED_YAML_DIR "${CMAKE_BINARY_DIR}/converted_yamls"

)

add_custom_target(convert_yaml_refs_installable)
set_target_properties(convert_yaml_refs_installable
    PROPERTIES
        EVEREST_CONVERTED_YAML_INSTALLABLE_DIR "${CMAKE_BINARY_DIR}/converted_yamls_installable"
)

# NOTE (aw): maybe this could be also implemented as an IMPORTED target?
add_custom_target(generate_cpp_files)
set_target_properties(generate_cpp_files
    PROPERTIES
        EVEREST_SCHEMA_DIR "${EVEREST_SCHEMA_DIR}"
        EVEREST_GENERATED_OUTPUT_DIR "${CMAKE_BINARY_DIR}/generated"
        EVEREST_GENERATED_INCLUDE_DIR "${CMAKE_BINARY_DIR}/generated/include"
        EVEREST_PROJECT_DIRS ""
)

#
# out-of-tree interfaces/types/modules support
#
function(ev_add_project)
    # FIXME (aw): resort to proper argument handling!
    if (ARGC EQUAL 2)
        set (EVEREST_PROJECT_DIR ${ARGV0})
        set (EVEREST_PROJECT_NAME ${ARGV1})
    endif ()

    if (NOT EVEREST_PROJECT_DIR)
        # assume current project dir
        set (EVEREST_PROJECT_DIR ${PROJECT_SOURCE_DIR})
        set (CALLED_FROM_WITHIN_PROJECT TRUE)
    elseif (NOT EXISTS ${EVEREST_PROJECT_DIR})
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} got non-existing project path: ${EVEREST_PROJECT_DIR}")
    endif ()

    if (NOT EVEREST_PROJECT_NAME)
        set (EVEREST_PROJECT_NAME ${PROJECT_NAME})
    endif ()

    # if we don't get a directory, we're assuming current source directory
    message(STATUS "APPENDING ${PROJECT_SOURCE_DIR} to EVEREST_PROJECT_DIRS")
    set_property(TARGET generate_cpp_files
        APPEND PROPERTY EVEREST_PROJECT_DIRS ${EVEREST_PROJECT_DIR}
    )
    get_target_property(EVEREST_PROJECT_DIRS generate_cpp_files EVEREST_PROJECT_DIRS)

    # check for types
    set(TYPES_DIR "${EVEREST_PROJECT_DIR}/types")
    if (EXISTS ${TYPES_DIR})
        message(STATUS "Adding type definitions from ${TYPES_DIR}")
        file(GLOB TYPES_FILES
            ${TYPES_DIR}/*.yaml
        )
        _ev_add_types(${TYPES_FILES})
    endif ()

    # check for errors
    set(ERRORS_DIR "${EVEREST_PROJECT_DIR}/errors")
    if (EXISTS ${ERRORS_DIR})
        message(STATUS "Adding error definitions from ${ERRORS_DIR}")
        file(GLOB ERRORS_FILES
            ${ERRORS_DIR}/*.yaml
        )
        foreach(ERRORS_FILE ${ERRORS_FILES})
            set(ERRORS_CONVERTED_REL_OUTPUT_DIR "errors")
            _ev_convert_refs(${ERRORS_FILE} ${ERRORS_CONVERTED_REL_OUTPUT_DIR})
        endforeach()
    endif ()

    # check for interfaces
    set (INTERFACES_DIR "${EVEREST_PROJECT_DIR}/interfaces")
    if (EXISTS ${INTERFACES_DIR})
        message(STATUS "Adding interface definitions from ${INTERFACES_DIR}")
        file(GLOB INTERFACE_FILES
            ${INTERFACES_DIR}/*.yaml
        )
        _ev_add_interfaces(${INTERFACE_FILES})
    endif ()

    # check for modules
    set (MODULES_DIR "${EVEREST_PROJECT_DIR}/modules")
    if (EXISTS "${MODULES_DIR}/CMakeLists.txt")
        # FIXME (aw): default handling of building all modules?
        if (EVC_MAIN_PROJECT OR NOT EVEREST_DONT_BUILD_ALL_MODULES)
            add_subdirectory(${MODULES_DIR})
        endif()
    endif ()
endfunction()

#
# convert relative references in yaml files to absolute references
#
function(_ev_convert_refs)
    if (ARGC EQUAL 2)
        set (CONVERT_REFS_INPUT_FILE ${ARGV0})
        set (CONVERT_REFS_REL_OUTPUT_DIR ${ARGV1})
    else()
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION}() expects exactly two arguments")
    endif()

    get_filename_component(CONVERT_REFS_INPUT_FILE_NAME ${CONVERT_REFS_INPUT_FILE} NAME)

    get_filename_component(CONVERT_REFS_INPUT_FILE_NAME_WITHOUT_EXT ${CONVERT_REFS_INPUT_FILE} NAME_WE)
    get_filename_component(CONVERT_REFS_INPUT_FILE_PARENT ${CONVERT_REFS_INPUT_FILE} DIRECTORY)
    get_filename_component(CONVERT_REFS_INPUT_FILE_PARENTNAME ${CONVERT_REFS_INPUT_FILE_PARENT} NAME_WE)
    set(CONVERT_REFS_TARGET_NAME "convert_yaml_refs_${CONVERT_REFS_INPUT_FILE_PARENT_NAME}_${CONVERT_REFS_INPUT_FILE_NAME_WITHOUT_EXT}")

    get_target_property(CONVERTED_YAML_DIR convert_yaml_refs EVEREST_CONVERTED_YAML_DIR)
    set(CONVERT_REFS_OUTPUT_FILE "${CONVERTED_YAML_DIR}/${CONVERT_REFS_REL_OUTPUT_DIR}/${CONVERT_REFS_INPUT_FILE_NAME}")
    add_custom_command(
        OUTPUT
            "${CONVERT_REFS_OUTPUT_FILE}"
        DEPENDS
            ${CONVERT_REFS_INPUT_FILE}
        COMMENT
            "Converting relative references in ${CONVERT_REFS_INPUT_FILE_NAME} to absolute references..."
        VERBATIM
        COMMAND
            ${EV_CLI} helpers convert-refs
                "${CONVERT_REFS_INPUT_FILE}"
                "${CONVERT_REFS_OUTPUT_FILE}"
        WORKING_DIRECTORY
            ${PROJECT_SOURCE_DIR}
    )
    add_custom_target( ${CONVERT_REFS_TARGET_NAME}
        DEPENDS
            "${CONVERT_REFS_OUTPUT_FILE}"
    )
    add_dependencies( convert_yaml_refs
        "${CONVERT_REFS_TARGET_NAME}"
    )

    # Install target
    set(CONVERT_REFS_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/everest/${CONVERT_REFS_REL_OUTPUT_DIR}")
    set(CONVERT_REFS_BASE_PATH "${CMAKE_INSTALL_PREFIX}/${CONVERT_REFS_INSTALL_DIR}/${CONVERT_REFS_INPUT_FILE_NAME}")

    get_target_property(CONVERTED_YAML_INSTALLABLE_DIR convert_yaml_refs_installable EVEREST_CONVERTED_YAML_INSTALLABLE_DIR)
    set(CONVERT_REFS_OUTPUT_FILE_INSTALLABLE "${CONVERTED_YAML_INSTALLABLE_DIR}/${CONVERT_REFS_REL_OUTPUT_DIR}/${CONVERT_REFS_INPUT_FILE_NAME}")
    add_custom_command(
        OUTPUT
            "${CONVERT_REFS_OUTPUT_FILE_INSTALLABLE}"
        DEPENDS
            ${CONVERT_REFS_INPUT_FILE}
        COMMENT
            "Converting relative references in ${CONVERT_REFS_INPUT_FILE_NAME} to absolute references..."
        VERBATIM
        COMMAND
            ${EV_CLI} helpers convert-refs
                "${CONVERT_REFS_INPUT_FILE}"
                "${CONVERT_REFS_OUTPUT_FILE_INSTALLABLE}"
                --base-path "${CONVERT_REFS_BASE_PATH}"
        WORKING_DIRECTORY
            ${PROJECT_SOURCE_DIR}
    )
    add_custom_target( ${CONVERT_REFS_TARGET_NAME}_installable
        ALL
        DEPENDS
            "${CONVERT_REFS_OUTPUT_FILE_INSTALLABLE}"
    )
    add_dependencies( convert_yaml_refs_installable
        "${CONVERT_REFS_TARGET_NAME}_installable"
    )
    if (CALLED_FROM_WITHIN_PROJECT)
        install(
            FILES
                "${CONVERT_REFS_OUTPUT_FILE_INSTALLABLE}"
            DESTINATION
                "${CONVERT_REFS_INSTALL_DIR}"
        )
    endif()
endfunction()

#
# interfaces
#
function (_ev_add_interfaces)
    # FIXME (aw): check for duplicates here!
    get_target_property(GENERATED_OUTPUT_DIR generate_cpp_files EVEREST_GENERATED_OUTPUT_DIR)
    set(CHECK_DONE_FILE "${GENERATED_OUTPUT_DIR}/.interfaces_generated_${EVEREST_PROJECT_NAME}")

    foreach(INTERFACE_PATH ${ARGV})
        get_filename_component(INTERFACE_FILENAME ${INTERFACE_PATH} NAME)
        set(INTERFACE_CONVERTED_REL_OUTPUT_DIR "interfaces")
        _ev_convert_refs(${INTERFACE_PATH} ${INTERFACE_CONVERTED_REL_OUTPUT_DIR})
    endforeach()

    add_custom_command(
        OUTPUT
            "${CHECK_DONE_FILE}"
        DEPENDS
            ${ARGV}
        COMMENT
            "Generating/updating interface files ..."
        VERBATIM
        COMMAND
            ${EV_CLI} interface generate-headers
                --disable-clang-format
                --schemas-dir "$<TARGET_PROPERTY:generate_cpp_files,EVEREST_SCHEMA_DIR>"
                --output-dir "$<TARGET_PROPERTY:generate_cpp_files,EVEREST_GENERATED_INCLUDE_DIR>/generated/interfaces"
                --everest-dir ${EVEREST_PROJECT_DIRS}
        COMMAND
            ${CMAKE_COMMAND} -E touch "${CHECK_DONE_FILE}"
        WORKING_DIRECTORY
            ${PROJECT_SOURCE_DIR}
    )

    add_custom_target(generate_interfaces_cpp_${EVEREST_PROJECT_NAME}
        DEPENDS "${CHECK_DONE_FILE}"
    )

    add_dependencies(generate_cpp_files
        generate_interfaces_cpp_${EVEREST_PROJECT_NAME}
    )
endfunction()

#
# types
#
function (_ev_add_types)
    # FIXME (aw): check for duplicates here!
    get_target_property(GENERATED_OUTPUT_DIR generate_cpp_files EVEREST_GENERATED_OUTPUT_DIR)
    set(CHECK_DONE_FILE "${GENERATED_OUTPUT_DIR}/.types_generated_${EVEREST_PROJECT_NAME}")

    foreach(TYPES_PATH ${ARGV})
        get_filename_component(TYPES_FILENAME ${TYPES_PATH} NAME)
        set(TYPES_CONVERTED_REL_OUTPUT_DIR "types")
        _ev_convert_refs(${TYPES_PATH} ${TYPES_CONVERTED_REL_OUTPUT_DIR})
    endforeach()

    add_custom_command(
        OUTPUT
            "${CHECK_DONE_FILE}"
        DEPENDS
            ${ARGV}
        COMMENT
            "Generating/updating type files ..."
        VERBATIM
        COMMAND
            ${EV_CLI} types generate-headers
                --disable-clang-format
                --schemas-dir "$<TARGET_PROPERTY:generate_cpp_files,EVEREST_SCHEMA_DIR>"
                --output-dir "$<TARGET_PROPERTY:generate_cpp_files,EVEREST_GENERATED_INCLUDE_DIR>/generated/types"
                --everest-dir ${EVEREST_PROJECT_DIRS}
        COMMAND
            ${CMAKE_COMMAND} -E touch "${CHECK_DONE_FILE}"
        WORKING_DIRECTORY 
            ${PROJECT_SOURCE_DIR}
    )

    add_custom_target(generate_types_cpp_${EVEREST_PROJECT_NAME}
        DEPENDS
            ${CHECK_DONE_FILE}
    )

    add_dependencies(generate_cpp_files
        generate_types_cpp_${EVEREST_PROJECT_NAME}
    )
endfunction()

#
# modules
#
function (ev_add_modules)
    set(EVEREST_MODULE_INSTALL_PREFIX "${CMAKE_INSTALL_LIBEXECDIR}/everest/modules")

    # NOTE (aw): this logic could be customized in future
    foreach(MODULE ${ARGV})
        if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${ENTRY})
            if("${MODULE}" IN_LIST EVEREST_EXCLUDE_MODULES)
                message(STATUS "Excluding module ${MODULE}")
            elseif(EVEREST_INCLUDE_MODULES AND NOT ("${MODULE}" IN_LIST EVEREST_INCLUDE_MODULES))
                message(STATUS "Excluding module ${MODULE}")
            else()
                message(WARNING "ev_add_modules is deprecated in favor of ev_add_<cpp/js/py>_module()")
                if("${MODULE}" MATCHES "^Js*")
                    ev_add_js_module(${MODULE})
                elseif("${MODULE}" MATCHES "^Py*")
                    ev_add_py_module(${MODULE})
                else()
                    ev_add_cpp_module(${MODULE})
                endif()
            endif()
        endif()
    endforeach()

    # FIXME (aw): this will override EVEREST_MODULES, might not what we want
    set_property(
        GLOBAL
        PROPERTY EVEREST_MODULES ${EVEREST_MODULES}
    )
endfunction()

function(ev_setup_cpp_module)
    # no-op to not break API
endfunction()

function (ev_add_module)
    #
    # handle passed arguments
    #
    set(options "")
    set(one_value_args "")
    set(multi_value_args
        DEPENDENCIES
    )

    if (${ARGC} LESS 1)
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION}() missing module name")
    endif ()

    set (MODULE_NAME ${ARGV0})

    cmake_parse_arguments(PARSE_ARGV 1 OPTNS "${options}" "${one_value_args}" "${multi_value_args}")

    if (OPTNS_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION}() got unknown argument(s): ${OPTNS_UNPARSED_ARGUMENTS}")
    endif()

    if (OPTNS_DEPENDENCIES)
        foreach(DEPENDENCY_NAME ${OPTNS_DEPENDENCIES})
            set(DEPENDENCY_VALUE ${${DEPENDENCY_NAME}})
            if (NOT DEPENDENCY_VALUE)
                message(STATUS "${FMT_BOLD}Skipping${FMT_RESET} module ${MODULE_NAME} (${DEPENDENCY_NAME} is false)")
                return()
            endif()
        endforeach()
    endif()

    # check if python module
    string(FIND ${MODULE_NAME} "Py" MODULE_PREFIX_POS)
    if (MODULE_PREFIX_POS EQUAL 0)
        ev_add_py_module(${MODULE_NAME})
        return()
    endif()

    # check if javascript module
    string(FIND ${MODULE_NAME} "Js" MODULE_PREFIX_POS)
    if (MODULE_PREFIX_POS EQUAL 0)
        ev_add_js_module(${MODULE_NAME})
        return()
    endif()

    # otherwise, should be cpp module
    ev_add_cpp_module(${MODULE_NAME})
endfunction()

function (ev_add_cpp_module MODULE_NAME)
    set(EVEREST_MODULE_INSTALL_PREFIX "${CMAKE_INSTALL_LIBEXECDIR}/everest/modules")
    set(EVEREST_MODULE_DIR ${PROJECT_SOURCE_DIR}/modules)

    file(RELATIVE_PATH MODULE_PARENT_DIR ${EVEREST_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

    set(RELATIVE_MODULE_DIR ${MODULE_NAME})
    if (MODULE_PARENT_DIR)
        set(RELATIVE_MODULE_DIR ${MODULE_PARENT_DIR}/${MODULE_NAME})
    endif()

    set(MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_NAME}")

    if(IS_DIRECTORY ${MODULE_PATH})
        if(${EVEREST_EXCLUDE_CPP_MODULES})
            message(STATUS "Excluding C++ module ${MODULE_NAME} because EVEREST_EXCLUDE_CPP_MODULES=${EVEREST_EXCLUDE_CPP_MODULES}")
            return()
        elseif("${MODULE_NAME}" IN_LIST EVEREST_EXCLUDE_MODULES)
            message(STATUS "Excluding module ${MODULE_NAME}")
            return()
        elseif(EVEREST_INCLUDE_MODULES AND NOT ("${MODULE_NAME}" IN_LIST EVEREST_INCLUDE_MODULES))
            message(STATUS "Excluding module ${MODULE_NAME}")
            return()
        else()
            message(STATUS "Setting up C++ module ${MODULE_NAME}")

            get_target_property(GENERATED_OUTPUT_DIR generate_cpp_files EVEREST_GENERATED_OUTPUT_DIR)

            set(GENERATED_MODULE_DIR "${GENERATED_OUTPUT_DIR}/modules")
            set(MODULE_LOADER_DIR ${GENERATED_MODULE_DIR}/${MODULE_NAME})

            add_custom_command(
                OUTPUT
                    ${MODULE_LOADER_DIR}/ld-ev.hpp
                    ${MODULE_LOADER_DIR}/ld-ev.cpp
                COMMAND
                    ${EV_CLI} module generate-loader
                        --disable-clang-format
                        --schemas-dir "$<TARGET_PROPERTY:generate_cpp_files,EVEREST_SCHEMA_DIR>"
                        --output-dir ${GENERATED_MODULE_DIR}
                        ${RELATIVE_MODULE_DIR}
                DEPENDS
                    ${MODULE_PATH}/manifest.yaml
                WORKING_DIRECTORY
                    ${PROJECT_SOURCE_DIR}
                COMMENT
                    "Generating ld-ev for module ${MODULE_NAME}"
            )

            add_custom_target(ld-ev_${MODULE_NAME}
                DEPENDS ${MODULE_LOADER_DIR}/ld-ev.cpp
            )

            add_dependencies(generate_cpp_files ld-ev_${MODULE_NAME})

            add_executable(${MODULE_NAME})

            set_target_properties(${MODULE_NAME}
                PROPERTIES
                    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${MODULE_NAME}"
            )

            target_include_directories(${MODULE_NAME}
                PRIVATE
                    ${MODULE_PATH}
                    "$<TARGET_PROPERTY:generate_cpp_files,EVEREST_GENERATED_INCLUDE_DIR>"
                    ${MODULE_LOADER_DIR}
            )

            target_sources(${MODULE_NAME}
                PRIVATE
                    ${MODULE_PATH}/${MODULE_NAME}.cpp
                    "${MODULE_LOADER_DIR}/ld-ev.cpp"
            )

            target_link_libraries(${MODULE_NAME}
                PRIVATE
                    everest::framework
            )

            add_dependencies(${MODULE_NAME} generate_cpp_files)

            install(TARGETS ${MODULE_NAME}
                DESTINATION "${EVEREST_MODULE_INSTALL_PREFIX}/${MODULE_NAME}"
            )

            install(FILES ${MODULE_PATH}/manifest.yaml
                DESTINATION "${EVEREST_MODULE_INSTALL_PREFIX}/${MODULE_NAME}"
            )

            list(APPEND MODULES ${MODULE_NAME})
            add_subdirectory(${MODULE_PATH})
        endif()
    else()
        message(WARNING "C++ module ${MODULE_NAME} does not exist at ${MODULE_PATH}")
        return()
    endif()

    # FIXME (aw): this will override EVEREST_MODULES, might not what we want
    set_property(
        GLOBAL
        PROPERTY EVEREST_MODULES ${EVEREST_MODULES}
    )
endfunction()

function (ev_add_js_module MODULE_NAME)
    set(EVEREST_MODULE_INSTALL_PREFIX "${CMAKE_INSTALL_LIBEXECDIR}/everest/modules")

    set(MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_NAME}")

    if(IS_DIRECTORY ${MODULE_PATH})
        if(NOT ${EVEREST_ENABLE_JS_SUPPORT})
            message(STATUS "Excluding JavaScript module ${MODULE_NAME} because EVEREST_ENABLE_JS_SUPPORT=${EVEREST_ENABLE_JS_SUPPORT}")
            return()
        elseif("${MODULE_NAME}" IN_LIST EVEREST_EXCLUDE_MODULES)
            message(STATUS "Excluding module ${MODULE_NAME}")
            return()
        elseif(EVEREST_INCLUDE_MODULES AND NOT ("${MODULE_NAME}" IN_LIST EVEREST_INCLUDE_MODULES))
            message(STATUS "Excluding module ${MODULE_NAME}")
            return()
        else()
            message(STATUS "Setting up JavaScript module ${MODULE_NAME}")

            add_custom_target(${MODULE_NAME} ALL)

            if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/package.json")
                message(STATUS "JavaScript module ${MODULE_NAME} contains a package.json file with dependencies that will be installed")

                add_dependencies(${MODULE_NAME} ${MODULE_NAME}_INSTALL_NODE_MODULES)

                find_program(
                    RSYNC
                    rsync
                    REQUIRED
                )

                find_program(
                    NPM
                    npm
                    REQUIRED
                )

                add_custom_command(
                    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/package.json
                    MAIN_DEPENDENCY package.json
                    COMMENT "Copy package.json of module ${MODULE_NAME} to build dir"
                    COMMAND ${RSYNC} -avq ${MODULE_PATH}/package.json ${CMAKE_CURRENT_BINARY_DIR}/package.json
                )

                add_custom_command(
                    OUTPUT .installed
                    MAIN_DEPENDENCY ${CMAKE_CURRENT_BINARY_DIR}/package.json
                    COMMENT "Installing dependencies of module ${MODULE_NAME} from package.json"
                    COMMAND ${NPM} install > npm.log 2>&1 || ${CMAKE_COMMAND} -E cat ${CMAKE_CURRENT_BINARY_DIR}/npm.log
                    COMMAND ${CMAKE_COMMAND} -E touch .installed
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                )

                add_custom_target(
                    ${MODULE_NAME}_INSTALL_NODE_MODULES
                    DEPENDS .installed
                )

                install(
                    DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/node_modules
                    DESTINATION "${EVEREST_MODULE_INSTALL_PREFIX}/${MODULE_NAME}"
                )
            endif()

            # install the whole js project
            if(CREATE_SYMLINKS)
                include("CreateModuleSymlink")
            else()
                install(
                    DIRECTORY ${MODULE_PATH}/
                    DESTINATION "${EVEREST_MODULE_INSTALL_PREFIX}/${MODULE_NAME}"
                    PATTERN "CMakeLists.txt" EXCLUDE
                    PATTERN "CMakeFiles" EXCLUDE)
            endif()

            list(APPEND MODULES ${MODULE_NAME})
            add_subdirectory(${MODULE_PATH})
        endif()
    else()
        message(WARNING "C++ module ${MODULE_NAME} does not exist at ${MODULE_PATH}")
        return()
    endif()

    # FIXME (aw): this will override EVEREST_MODULES, might not what we want
    set_property(
        GLOBAL
        PROPERTY EVEREST_MODULES ${EVEREST_MODULES}
    )
endfunction()

function (ev_add_py_module MODULE_NAME)
    set(EVEREST_MODULE_INSTALL_PREFIX "${CMAKE_INSTALL_LIBEXECDIR}/everest/modules")

    set(MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_NAME}")

    if(IS_DIRECTORY ${MODULE_PATH})
        if(NOT ${EVEREST_ENABLE_PY_SUPPORT})
            message(STATUS "Excluding Python module ${MODULE_NAME} because EVEREST_ENABLE_PY_SUPPORT=${EVEREST_ENABLE_PY_SUPPORT}")
            return()
        elseif("${MODULE_NAME}" IN_LIST EVEREST_EXCLUDE_MODULES)
            message(STATUS "Excluding module ${MODULE_NAME}")
            return()
        elseif(EVEREST_INCLUDE_MODULES AND NOT ("${MODULE_NAME}" IN_LIST EVEREST_INCLUDE_MODULES))
            message(STATUS "Excluding module ${MODULE_NAME}")
            return()
        else()
            message(STATUS "Setting up Python module ${MODULE_NAME}")

            add_custom_target(${MODULE_NAME} ALL)

            # TODO: figure out how to properly install python dependencies

            # install the whole python project
            install(
                DIRECTORY ${MODULE_PATH}/
                DESTINATION "${EVEREST_MODULE_INSTALL_PREFIX}/${MODULE_NAME}"
                PATTERN "CMakeLists.txt" EXCLUDE
                PATTERN "CMakeFiles" EXCLUDE)

            list(APPEND MODULES ${MODULE_NAME})
            add_subdirectory(${MODULE_PATH})
        endif()
    else()
        message(WARNING "C++ module ${MODULE_NAME} does not exist at ${MODULE_PATH}")
        return()
    endif()

    # FIXME (aw): this will override EVEREST_MODULES, might not what we want
    set_property(
        GLOBAL
        PROPERTY EVEREST_MODULES ${EVEREST_MODULES}
    )
endfunction()

function(ev_install_project)
    set (LIBRARY_PACKAGE_NAME ${PROJECT_NAME})
    
    include(CMakePackageConfigHelpers)

    set (EVEREST_DATADIR "${CMAKE_INSTALL_DATADIR}/everest")

    configure_package_config_file(
        ${EV_CORE_CMAKE_SCRIPT_DIR}/project-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_PACKAGE_NAME}-config.cmake
        INSTALL_DESTINATION
            ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_PACKAGE_NAME}
        PATH_VARS
            EVEREST_DATADIR
    )

    install(
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_PACKAGE_NAME}-config.cmake
            ${EV_CORE_CMAKE_SCRIPT_DIR}/everest-generate.cmake
            ${EV_CORE_CMAKE_SCRIPT_DIR}/ev-cli.cmake
            ${EV_CORE_CMAKE_SCRIPT_DIR}/project-config.cmake.in
            ${EV_CORE_CMAKE_SCRIPT_DIR}/ev-project-bootstrap.cmake
            ${EV_CORE_CMAKE_SCRIPT_DIR}/config-run-script.cmake
            ${EV_CORE_CMAKE_SCRIPT_DIR}/config-run-nodered-script.cmake
        DESTINATION
            ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_PACKAGE_NAME}
    )
endfunction()

set(EVEREST_EXCLUDE_MODULES "" CACHE STRING "A list of modules that will not be built")
set(EVEREST_INCLUDE_MODULES "" CACHE STRING "A list of modules that will be built. If the list is empty, all modules will be built.")
option(EVEREST_EXCLUDE_CPP_MODULES "Exclude all C++ modules from the build" OFF)
