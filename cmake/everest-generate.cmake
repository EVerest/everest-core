if (NOT EVEREST_SCHEMA_DIR)
    get_filename_component(CUR_FILE_NAME ${CMAKE_CURRENT_LIST_FILE} NAME)
    message(FATAL_ERROR "\
The variable EVEREST_SCHEMA_DIR is not set, this needs to be done, \
before including \"${CUR_FILE_NAME}\"\
    ")
endif()

set (EV_CORE_CMAKE_SCRIPT_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE FILEPATH "")

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

        if (CALLED_FROM_WITHIN_PROJECT)
            install(
                DIRECTORY ${TYPES_DIR}
                DESTINATION "${CMAKE_INSTALL_DATADIR}/everest"
                FILES_MATCHING PATTERN "*.yaml"
            )
        endif ()
    endif ()

    # check for interfaces
    set (INTERFACES_DIR "${EVEREST_PROJECT_DIR}/interfaces")
    if (EXISTS ${INTERFACES_DIR})
        message(STATUS "Adding interface definitions from ${INTERFACES_DIR}")
        file(GLOB INTERFACE_FILES
            ${INTERFACES_DIR}/*.yaml
        )

        _ev_add_interfaces(${INTERFACE_FILES})

        if (CALLED_FROM_WITHIN_PROJECT)
            install(
                DIRECTORY ${INTERFACES_DIR}
                DESTINATION "${CMAKE_INSTALL_DATADIR}/everest"
                FILES_MATCHING PATTERN "*.yaml"
            )
        endif ()
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
# interfaces
#
function (_ev_add_interfaces)
    # FIXME (aw): check for duplicates here!
    get_target_property(GENERATED_OUTPUT_DIR generate_cpp_files EVEREST_GENERATED_OUTPUT_DIR)
    set(CHECK_DONE_FILE "${GENERATED_OUTPUT_DIR}/.interfaces_generated_${EVEREST_PROJECT_NAME}")

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

function (ev_add_cpp_module MODULE_NAME)

    if (ARGC GREATER 1)
        set(MODULE_SUBDIR ${ARGV1})
        message (STATUS "Module is located in a sub directory: ${MODULE_SUBDIR}")
    else()
        set(MODULE_SUBDIR ".")
    endif ()

    set(EVEREST_MODULE_INSTALL_PREFIX "${CMAKE_INSTALL_LIBEXECDIR}/everest/modules")

    set(MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_SUBDIR}/${MODULE_NAME}")

    if(IS_DIRECTORY ${MODULE_PATH})
        if(${EVEREST_EXCLUDE_CPP_MODULES})
            message(STATUS "Excluding C++ module ${MODULE_NAME} because EVEREST_EXCLUDE_CPP_MODULES=${EVEREST_EXCLUDE_CPP_MODULES}")
            return()
        elseif("${MODULE_NAME}" IN_LIST EVEREST_EXCLUDE_MODULES)
            message(STATUS "Excluding module ${MODULE}")
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
                        ${MODULE_NAME}
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

    if (ARGC GREATER 1)
        set(MODULE_SUBDIR ${ARGV1})
        message (STATUS "Module is located in a sub directory: ${MODULE_SUBDIR}")
    else()
        set(MODULE_SUBDIR ".")
    endif ()

    set(EVEREST_MODULE_INSTALL_PREFIX "${CMAKE_INSTALL_LIBEXECDIR}/everest/modules")

    set(MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_SUBDIR}/${MODULE_NAME}")

    if(IS_DIRECTORY ${MODULE_PATH})
        if(NOT ${EVEREST_ENABLE_JS_SUPPORT})
            message(STATUS "Excluding JavaScript module ${MODULE_NAME} because EVEREST_ENABLE_JS_SUPPORT=${EVEREST_ENABLE_JS_SUPPORT}")
            return()
        elseif("${MODULE_NAME}" IN_LIST EVEREST_EXCLUDE_MODULES)
            message(STATUS "Excluding module ${MODULE}")
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

    if (ARGC GREATER 1)
        set(MODULE_SUBDIR ${ARGV1})
        message (STATUS "Module is located in a sub directory: ${MODULE_SUBDIR}")
    else()
        set(MODULE_SUBDIR ".")
    endif ()

    set(EVEREST_MODULE_INSTALL_PREFIX "${CMAKE_INSTALL_LIBEXECDIR}/everest/modules")

    set(MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_SUBDIR}/${MODULE_NAME}")

    if(IS_DIRECTORY ${MODULE_PATH})
        if(NOT ${EVEREST_ENABLE_PY_SUPPORT})
            message(STATUS "Excluding Python module ${MODULE_NAME} because EVEREST_ENABLE_PY_SUPPORT=${EVEREST_ENABLE_PY_SUPPORT}")
            return()
        elseif("${MODULE_NAME}" IN_LIST EVEREST_EXCLUDE_MODULES)
            message(STATUS "Excluding module ${MODULE}")
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
option(EVEREST_EXCLUDE_CPP_MODULES "Exclude all C++ modules from the build" OFF)
