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
function(_ev_add_project)
    set(options SKIP_DOC_GENERATION)
    set(oneValueArgs EV_PROJECT_DIRECTORY EV_PROJECT_NAME)
    set(multiValueArgs "")
    cmake_parse_arguments(args "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (args_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "_ev_add_project function got unknown argument(s): ${args_UNPARSED_ARGUMENTS}")
    endif()

    if(args_KEYWORDS_MISSING_VALUES)
        message(FATAL_ERROR "ev_add_project() keyword(s) missing values: ${args_KEYWORDS_MISSING_VALUES}")
    endif()

    if(args_EV_PROJECT_DIRECTORY AND args_EV_PROJECT_NAME)
        set (EVEREST_PROJECT_DIR ${args_EV_PROJECT_DIRECTORY})
        set (EVEREST_PROJECT_NAME ${args_EV_PROJECT_NAME})
    elseif(NOT args_PROJECT_NAME AND NOT args_PROJECT_DIRECTORY)
        # if we don't get a directory, we're assuming project directory
        set (EVEREST_PROJECT_DIR ${PROJECT_SOURCE_DIR})
        set (CALLED_FROM_WITHIN_PROJECT TRUE)
        set (EVEREST_PROJECT_NAME ${PROJECT_NAME})
    else()
        message(FATAL_ERROR "ev_add_project() can only be called with ALL or NONE of: 'EV_PROJECT_DIRECTORY', 'EV_PROJECT_NAME'")
    endif()

    if (NOT EXISTS ${EVEREST_PROJECT_DIR})
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} got non-existing project path: ${EVEREST_PROJECT_DIR}")
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

        if(EVEREST_BUILD_DOCS AND NOT args_SKIP_DOC_GENERATION)
            find_package(
                trailbook-ext-everest
                0.1.0
                REQUIRED
                PATHS "${CMAKE_SOURCE_DIR}/cmake"
            )
            foreach(TYPES_FILE ${TYPES_FILES})
                trailbook_ev_generate_rst_from_types(
                    TRAILBOOK_NAME "everest"
                    TYPES_FILE "${TYPES_FILE}"
                )
            endforeach()
        endif()

        _ev_add_types(${TYPES_FILES})

        if (CALLED_FROM_WITHIN_PROJECT)
            install(
                DIRECTORY ${TYPES_DIR}
                DESTINATION "${CMAKE_INSTALL_DATADIR}/everest"
                FILES_MATCHING PATTERN "*.yaml"
            )
        endif ()
    endif ()

    # check for API
    set(API_DIR "${EVEREST_PROJECT_DIR}/docs/source/reference/EVerest_API")
    if (EXISTS ${API_DIR})
        if (${EVEREST_SKIP_BUILD_API_DOC})
            message(WARNING "Skipping the generation of the EVerest API AsyncAPI html documentation")
        else()
            message(STATUS "Adding API definitions from ${API_DIR}")
            file(GLOB API_FILES
                ${API_DIR}/*.yaml
            )

            if(EVEREST_BUILD_DOCS AND NOT args_SKIP_DOC_GENERATION)
                find_package(
                    trailbook-ext-everest
                    0.1.0
                    REQUIRED
                    PATHS "${CMAKE_SOURCE_DIR}/cmake"
                )
                trailbook_ev_generate_api_doc(
                    TRAILBOOK_NAME "everest"
                    API_FILES ${API_FILES}
                )
            endif()

            if (CALLED_FROM_WITHIN_PROJECT)
                install(
                    DIRECTORY ${API_DIR}
                    DESTINATION "${CMAKE_INSTALL_DATADIR}/everest"
                    FILES_MATCHING PATTERN "*.yaml"
                )
            endif ()
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

        if(EVEREST_BUILD_DOCS AND NOT args_SKIP_DOC_GENERATION)
            find_package(
                trailbook-ext-everest
                0.1.0
                REQUIRED
                PATHS "${CMAKE_SOURCE_DIR}/cmake"
            )
            foreach(INTERFACE_FILE ${INTERFACE_FILES})
                trailbook_ev_generate_rst_from_interface(
                    TRAILBOOK_NAME "everest"
                    INTERFACE_FILE "${INTERFACE_FILE}"
                )
            endforeach()
        endif()

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

macro(ev_add_project)
    set(options SKIP_DOC_GENERATION)
    set(oneValueArgs EV_PROJECT_DIRECTORY EV_PROJECT_NAME)
    set(multiValueArgs "")
    cmake_parse_arguments(macro_args "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (macro_args_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "ev_add_project macro got unknown argument(s): ${macro_args_UNPARSED_ARGUMENTS}")
    endif()

    if(macro_args_KEYWORDS_MISSING_VALUES)
        message(FATAL_ERROR "ev_add_project() keyword(s) missing values: ${macro_args_KEYWORDS_MISSING_VALUES}")
    endif()

    if(macro_args_EV_PROJECT_DIRECTORY AND NOT macro_args_EV_PROJECT_NAME)
        message(FATAL_ERROR "ev_add_project() was called with EV_PROJECT_DIRECTORY but is missing EV_PROJECT_NAME.")
    elseif(NOT macro_args_EV_PROJECT_DIRECTORY AND macro_args_EV_PROJECT_NAME)
        message(FATAL_ERROR "ev_add_project() was called with EV_PROJECT_NAME but is missing EV_PROJECT_DIRECTORY.")
    endif()

    ev_setup_cmake_variables_python_wheel()
    set(${PROJECT_NAME}_PYTHON_VENV_PATH "${CMAKE_BINARY_DIR}/venv" CACHE PATH "Path to python venv")

    ev_setup_python_executable(
        USE_PYTHON_VENV ${${PROJECT_NAME}_USE_PYTHON_VENV}
        PYTHON_VENV_PATH ${${PROJECT_NAME}_PYTHON_VENV_PATH}
    )

    setup_ev_cli()

    if (${macro_args_SKIP_DOC_GENERATION})
        set (macro_fwd_OPTION "SKIP_DOC_GENERATION")
    else()
        set (macro_fwd_OPTION "")
    endif()

    if (macro_args_EV_PROJECT_DIRECTORY AND macro_args_EV_PROJECT_NAME)
        _ev_add_project(
            EV_PROJECT_DIRECTORY ${macro_args_EV_PROJECT_DIRECTORY}
            EV_PROJECT_NAME ${macro_args_EV_PROJECT_NAME}
            ${macro_fwd_OPTION}
        )
    else()
        _ev_add_project(${macro_fwd_OPTION})
    endif ()
endmacro()

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
            echo "everestrs = { path = \"$<TARGET_PROPERTY:everest::everestrs_sys,EVERESTRS_DIR>\" }" >> Cargo.toml
        COMMAND
            echo "everestrs-build = { path = \"$<TARGET_PROPERTY:everest::everestrs_sys,EVERESTRS_BUILD_DIR>\" }" >> Cargo.toml
        WORKING_DIRECTORY
            ${RUST_WORKSPACE_DIR}
        VERBATIM
        DEPENDS
            ${RUST_WORKSPACE_DIR}
    )

    # Put the resulting file in the top-level build directory so that it can be easily accessed without CMake
    set(RUST_LINK_DEPENDENCIES_FILE ${CMAKE_BINARY_DIR}/everestrs-link-dependencies.txt)
    set(RUST_LINK_DEPENDENCIES "$<TARGET_GENEX_EVAL:everest::everestrs_sys,$<TARGET_PROPERTY:everest::everestrs_sys,EVERESTRS_LINK_DEPENDENCIES>>")

    add_custom_command(OUTPUT ${RUST_LINK_DEPENDENCIES_FILE}
        COMMAND_EXPAND_LISTS
        VERBATIM
        COMMAND
            echo -e $<LIST:JOIN,${RUST_LINK_DEPENDENCIES},\\n> > "${RUST_LINK_DEPENDENCIES_FILE}"
    )

    add_custom_target(generate_rust
        DEPENDS
            ${RUST_WORKSPACE_CARGO_FILE}
            ${RUST_LINK_DEPENDENCIES_FILE}
    )

    # Store the workspace directory as a target property so that it is accessible in different scopes
    set_property(TARGET generate_rust
        PROPERTY
            RUST_WORKSPACE_DIR "${RUST_WORKSPACE_DIR}"
    )

    # FIXME (aw): use generator expressions here, but this first needs to be fixed in the build.rs file ...
    add_custom_target(build_rust_modules ALL
        USES_TERMINAL
        COMMENT
            "Build rust modules"
        COMMAND
            ${CMAKE_COMMAND} -E env
            EVEREST_CORE_ROOT="${CMAKE_CURRENT_SOURCE_DIR}"
            EVEREST_RS_LINK_DEPENDENCIES="${RUST_LINK_DEPENDENCIES_FILE}"
            ${CARGO_EXECUTABLE} build
            $<IF:$<STREQUAL:$<CONFIG>,Release>,--release,>
            # explicitly set the linker to match what we're using for C++ to avoid the following issue when cross compiling:
            # https://github.com/rust-lang/rust/issues/28924
            --config 'target.$<TARGET_PROPERTY:build_rust_modules,RUST_TARGET_TRIPLE>.linker = \"${CMAKE_CXX_COMPILER}\"'
            --target $<TARGET_PROPERTY:build_rust_modules,RUST_TARGET_TRIPLE>
        WORKING_DIRECTORY
            ${RUST_WORKSPACE_DIR}
        DEPENDS
            everest::everestrs_sys
            generate_rust
    )

    # FIXME: cleaning up doesn't work on the first run
    set_property(TARGET build_rust_modules
        APPEND
        PROPERTY
            ADDITIONAL_CLEAN_FILES ${RUST_WORKSPACE_DIR}/target ${RUST_WORKSPACE_DIR}/Cargo.lock
    )

    set_property(TARGET build_rust_modules
        PROPERTY
            # FIXME: Don't assume the glibc ABI here. This won't respect musl builds.
            RUST_TARGET_TRIPLE "${CMAKE_SYSTEM_PROCESSOR}-unknown-linux-gnu"
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
        get_target_property(RUST_WORKSPACE_DIR generate_rust RUST_WORKSPACE_DIR)

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
        set(BIN_PREFIX "target/$<TARGET_PROPERTY:build_rust_modules,RUST_TARGET_TRIPLE>/$<IF:$<STREQUAL:$<CONFIG>,Release>,release,debug>")

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
            "$<TARGET_PROPERTY:ev-cli,INTERFACE_TEMPLATES>"
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
            "$<TARGET_PROPERTY:ev-cli,TYPES_TEMPLATES>"
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
    set(options SKIP_DOC_GENERATION)
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

    if (EVEREST_BUILD_DOCS AND NOT OPTNS_SKIP_DOC_GENERATION)
        find_package(
            trailbook-ext-everest
            0.1.0
            REQUIRED
            PATHS "${CMAKE_SOURCE_DIR}/cmake"
        )
        if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_NAME}/docs/")
            trailbook_ev_add_module_handwritten_doc(
                TRAILBOOK_NAME "everest"
                MODULE_NAME "${MODULE_NAME}"
                HANDWRITTEN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_NAME}/docs"
            )
        endif()
        if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_NAME}/doc.rst")
            message(
                FATAL_ERROR
                "Module ${MODULE_NAME} contains a doc.rst file"
                " this is not supported anymore, please move to"
                " docs/index.rst.inc, then it will be picked up automatically."
                " For now this file will be ignored."
            )
        endif()
        trailbook_ev_generate_rst_from_manifest(
            TRAILBOOK_NAME "everest"
            MANIFEST_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_NAME}/manifest.yaml"
        )
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
                    "$<TARGET_PROPERTY:ev-cli,MODULE_TEMPLATES>"
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
                    ${ATOMIC_LIBS}
            )

            if(EVEREST_ENABLE_COMPILE_WARNINGS)
                message(STATUS "Building ${MODULE_NAME} with the following compile options: ${EVEREST_COMPILE_OPTIONS}")
                target_compile_options(${MODULE_NAME}
                    PRIVATE ${EVEREST_COMPILE_OPTIONS}
                )
            endif()

            add_dependencies(${MODULE_NAME} generate_cpp_files)

            ev_register_module_target(${MODULE_NAME})

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
            ${EV_CORE_CMAKE_SCRIPT_DIR}/ev-targets.cmake
            ${EV_CORE_CMAKE_SCRIPT_DIR}/config-run-script.cmake
            ${EV_CORE_CMAKE_SCRIPT_DIR}/config-run-nodered-script.cmake
            ${EV_CORE_CMAKE_SCRIPT_DIR}/config-tmux-run-script.cmake
        DESTINATION
            ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_PACKAGE_NAME}
    )
endfunction()

set(EVEREST_EXCLUDE_MODULES "" CACHE STRING "A list of modules that will not be built")
set(EVEREST_INCLUDE_MODULES "" CACHE STRING "A list of modules that will be built. If the list is empty, all modules will be built.")
option(EVEREST_EXCLUDE_CPP_MODULES "Exclude all C++ modules from the build" OFF)
