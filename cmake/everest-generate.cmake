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
        # if we don't get a directory, we're assuming project directory
        set (EVEREST_PROJECT_DIR ${PROJECT_SOURCE_DIR})
        set (CALLED_FROM_WITHIN_PROJECT TRUE)
    elseif (NOT EXISTS ${EVEREST_PROJECT_DIR})
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} got non-existing project path: ${EVEREST_PROJECT_DIR}")
    endif ()

    if (NOT EVEREST_PROJECT_NAME)
        set (EVEREST_PROJECT_NAME ${PROJECT_NAME})
    endif ()

    message(STATUS "APPENDING ${EVEREST_PROJECT_DIR} to EVEREST_PROJECT_DIRS")
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

    # check for errors
    set(ERRORS_DIR "${EVEREST_PROJECT_DIR}/errors")
    if (EXISTS ${ERRORS_DIR})
        message(STATUS "Adding error definitions from ${ERRORS_DIR}")
        if (CALLED_FROM_WITHIN_PROJECT)
            install(
                DIRECTORY ${ERRORS_DIR}
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

    get_property(EVEREST_MODULES
        GLOBAL
        PROPERTY EVEREST_MODULES
    )
    message(STATUS "${EVEREST_PROJECT_NAME} modules that will be built: ${EVEREST_MODULES}")

    # generate and install version information
    evc_generate_version_information()
    install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/generated/version_information.txt
        DESTINATION "${CMAKE_INSTALL_DATADIR}/everest"
    )
endfunction()

#
# rust support
#
# FIXME (aw): move this stuff to some other cmake file for more modularity
if (EVEREST_ENABLE_RS_SUPPORT)
    find_program(CARGO_EXECUTABLE cargo REQUIRED)

    # FIXME (aw): the RUST_WORKSPACE_DIR could be user setable!
    set(RUST_WORKSPACE_DIR ${PROJECT_BINARY_DIR}/rust_workspace)
    set(RUST_WORKSPACE_CARGO_FILE ${RUST_WORKSPACE_DIR}/Cargo.toml)

    if (NOT EXISTS ${RUST_WORKSPACE_DIR})
        execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${RUST_WORKSPACE_DIR}")
        message(STATUS "Creating rust workspace at ${RUST_WORKSPACE_DIR}")
    endif ()

    if (EVEREST_CORE_BUILD_TESTING)
        set(EVERESTRS_FEATURE_FLAGS ",features = [\"link_gcov\"]")
    else()
        set(EVERESTRS_FEATURE_FLAGS "")
    endif()

    # NOTE (aw): we could also write a small python script, which would do that for us
    add_custom_command(OUTPUT ${RUST_WORKSPACE_CARGO_FILE}
        COMMAND
            echo "[workspace]" > Cargo.toml
        COMMAND
            echo "resolver = \"2\"" >> Cargo.toml
        COMMAND
            echo "members = [" >> Cargo.toml
        COMMAND
            echo "  \"$<JOIN:$<TARGET_PROPERTY:generate_rust,RUST_MODULE_LIST>,\", \">\"," >> Cargo.toml  # :)
        COMMAND
            echo "]" >> Cargo.toml && echo "" >> Cargo.toml
        COMMAND
            echo "[workspace.dependencies]" >> Cargo.toml
        COMMAND
            echo "everestrs = { path = \"$<TARGET_PROPERTY:everest::everestrs_sys,EVERESTRS_DIR>\" ${EVERESTRS_FEATURE_FLAGS} }" >> Cargo.toml
        COMMAND
            echo "everestrs-build = { path = \"$<TARGET_PROPERTY:everest::everestrs_sys,EVERESTRS_BUILD_DIR>\" }" >> Cargo.toml
        COMMAND
            echo $<TARGET_FILE:everest::framework> > .everestrs_link_dependencies
        COMMAND
            echo $<TARGET_FILE:everest::log> >> .everestrs_link_dependencies
        WORKING_DIRECTORY
            ${RUST_WORKSPACE_DIR}
        VERBATIM
        DEPENDS
            ${RUST_WORKSPACE_DIR}
    )

    add_custom_target(generate_rust
        DEPENDS
            ${RUST_WORKSPACE_CARGO_FILE}
    )

    # FIXME (aw): use generator expressions here, but this first needs to be fixed in the build.rs file ...
    add_custom_target(build_rust_modules ALL
        COMMENT
            "Build rust modules"
        COMMAND
            ${CMAKE_COMMAND} -E env
            EVEREST_CORE_ROOT="${CMAKE_CURRENT_SOURCE_DIR}"
            EVEREST_RS_FRAMEWORK_SOURCE_LOCATION="${everest-framework_SOURCE_DIR}"
            EVEREST_RS_FRAMEWORK_BINARY_LOCATION="${everest-framework_BINARY_DIR}"
            ${CARGO_EXECUTABLE} build
            $<IF:$<STREQUAL:${CMAKE_BUILD_TYPE},Release>,--release,>
        WORKING_DIRECTORY
            ${RUST_WORKSPACE_DIR}
        DEPENDS
            everestrs_sys
            generate_rust
    )

    # FIXME: cleaning up doesn't work on the first run
    set_property(TARGET build_rust_modules
        APPEND
        PROPERTY
            ADDITIONAL_CLEAN_FILES ${RUST_WORKSPACE_DIR}/target ${RUST_WORKSPACE_DIR}/Cargo.lock
    )

    function (ev_add_rs_module MODULE_NAME)
        if(NOT ${EVEREST_ENABLE_RS_SUPPORT})
            message(STATUS "Excluding Rust module ${MODULE_NAME} because EVEREST_ENABLE_RS_SUPPORT=${EVEREST_ENABLE_RS_SUPPORT}")
            return()
        elseif ("${MODULE_NAME}" IN_LIST EVEREST_EXCLUDE_MODULES)
            message(STATUS "Excluding module ${MODULE_NAME}")
            return()
        elseif (EVEREST_INCLUDE_MODULES AND NOT ("${MODULE_NAME}" IN_LIST EVEREST_INCLUDE_MODULES))
            message(STATUS "Excluding module ${MODULE_NAME}")
            return()
        endif ()

        set(MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_NAME}")
        if (NOT IS_DIRECTORY ${MODULE_PATH})
            message(FATAL "Rust module ${MODULE_NAME} does not exist at ${MODULE_PATH}")
            return()
        endif ()

        message(STATUS "Setting up Rust module ${MODULE_NAME}")

        # FIXME (aw): we might also look for a CMakeFiles.txt in the module folder for custom logic
        set_property(
            TARGET generate_rust
            APPEND
            PROPERTY RUST_MODULE_LIST "${MODULE_NAME}"
        )

        add_custom_command(OUTPUT ${RUST_WORKSPACE_DIR}/${MODULE_NAME}
            COMMAND
                ${CMAKE_COMMAND} -E create_symlink ${MODULE_PATH} ${MODULE_NAME}
            COMMENT
                "Create symlink for rust module ${MODULE_NAME}"
            VERBATIM
            WORKING_DIRECTORY
                ${RUST_WORKSPACE_DIR}
        )

        add_custom_target(rust_symlink_module_${MODULE_NAME}
            DEPENDS ${RUST_WORKSPACE_DIR}/${MODULE_NAME}
            COMMENT "Create symlink for rust module ${MODULE_NAME}"
        )

        add_dependencies(generate_rust rust_symlink_module_${MODULE_NAME})

        set(EVEREST_MODULE_INSTALL_PREFIX "${CMAKE_INSTALL_LIBEXECDIR}/everest/modules")

        if (CMAKE_BUILD_TYPE STREQUAL "Release")
            set(BIN_PREFIX "target/release")
        else ()
            set(BIN_PREFIX "target/debug")
        endif ()

        install(PROGRAMS ${RUST_WORKSPACE_DIR}/${BIN_PREFIX}/${MODULE_NAME}
            DESTINATION "${EVEREST_MODULE_INSTALL_PREFIX}/${MODULE_NAME}"
        )

        # FIXME (aw): this should go into a general function for all add_module_* flavours
        install(FILES ${MODULE_PATH}/manifest.yaml
            DESTINATION "${EVEREST_MODULE_INSTALL_PREFIX}/${MODULE_NAME}"
        )
    endfunction()

endif () # EVEREST_ENABLE_RS_SUPPORT


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

    # check if rust module
    string(FIND ${MODULE_NAME} "Rs" MODULE_PREFIX_POS)
    if (MODULE_PREFIX_POS EQUAL 0)
        if (NOT EVEREST_ENABLE_RS_SUPPORT)
            return() # NOTE (aw): could log here
        endif ()

        ev_add_rs_module(${MODULE_NAME})
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

    get_property(EVEREST_MODULES
        GLOBAL
        PROPERTY EVEREST_MODULES
    )

    # TODO(hikinggrass): This code is duplicated in ev_add_*_module and should be refactored.
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
                    ${CMAKE_BINARY_DIR}
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
                    ${ATOMIC_LIBS}
            )

            add_dependencies(${MODULE_NAME} generate_cpp_files)

            install(TARGETS ${MODULE_NAME}
                DESTINATION "${EVEREST_MODULE_INSTALL_PREFIX}/${MODULE_NAME}"
            )

            install(FILES ${MODULE_PATH}/manifest.yaml
                DESTINATION "${EVEREST_MODULE_INSTALL_PREFIX}/${MODULE_NAME}"
            )

            list(APPEND EVEREST_MODULES ${MODULE_NAME})
            add_subdirectory(${MODULE_PATH})
        endif()
    else()
        message(WARNING "C++ module ${MODULE_NAME} does not exist at ${MODULE_PATH}")
        return()
    endif()

    # this will override EVEREST_MODULES, but that is ok because we appended the list earlier
    # rename EVEREST_MODULES to EVEREST_MODULES
    # use set_property APPEND
    set_property(
        GLOBAL
        PROPERTY EVEREST_MODULES ${EVEREST_MODULES}
    )
endfunction()

function (ev_add_js_module MODULE_NAME)
    set(EVEREST_MODULE_INSTALL_PREFIX "${CMAKE_INSTALL_LIBEXECDIR}/everest/modules")

    set(MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_NAME}")

    get_property(EVEREST_MODULES
        GLOBAL
        PROPERTY EVEREST_MODULES
    )

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

            list(APPEND EVEREST_MODULES ${MODULE_NAME})
            add_subdirectory(${MODULE_PATH})
        endif()
    else()
        message(WARNING "JavaScript module ${MODULE_NAME} does not exist at ${MODULE_PATH}")
        return()
    endif()

    # this will override EVEREST_MODULES, but that is ok because we appended the list earlier
    set_property(
        GLOBAL
        PROPERTY EVEREST_MODULES ${EVEREST_MODULES}
    )
endfunction()

function (ev_add_py_module MODULE_NAME)
    set(EVEREST_MODULE_INSTALL_PREFIX "${CMAKE_INSTALL_LIBEXECDIR}/everest/modules")

    set(MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_NAME}")

    get_property(EVEREST_MODULES
        GLOBAL
        PROPERTY EVEREST_MODULES
    )

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

            list(APPEND EVEREST_MODULES ${MODULE_NAME})
            add_subdirectory(${MODULE_PATH})
        endif()
    else()
        message(WARNING "Python module ${MODULE_NAME} does not exist at ${MODULE_PATH}")
        return()
    endif()

    # this will override EVEREST_MODULES, but that is ok because we appended the list earlier
    set_property(
        GLOBAL
        PROPERTY EVEREST_MODULES ${EVEREST_MODULES}
    )
endfunction()

function(ev_install_project)
    set (LIBRARY_PACKAGE_NAME ${PROJECT_NAME})
    set (LIBRARY_PACKAGE_CMAKE_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_PACKAGE_NAME})

    include(CMakePackageConfigHelpers)

    set (EVEREST_DATADIR "${CMAKE_INSTALL_DATADIR}/everest")

    configure_package_config_file(
        ${EV_CORE_CMAKE_SCRIPT_DIR}/project-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_PACKAGE_NAME}-config.cmake
        INSTALL_DESTINATION
            ${LIBRARY_PACKAGE_CMAKE_INSTALL_DIR}
        PATH_VARS
            EVEREST_DATADIR
    )

    install(
        EXPORT everest-core-targets
        FILE "everest-core-targets.cmake"
        NAMESPACE everest::
        DESTINATION ${LIBRARY_PACKAGE_CMAKE_INSTALL_DIR}
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
