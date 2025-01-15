include_guard(GLOBAL)

set(EVEREST_CORE_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}")

function(_ev_create_version_library PROJECT_NAME)
    set(VERSION_LIBRARY "${PROJECT_NAME}_version")

    add_library(${VERSION_LIBRARY})

    target_sources(${VERSION_LIBRARY} PRIVATE
        ${EVEREST_CORE_CMAKE_DIR}/assets/version.cpp
    )

    target_include_directories(${VERSION_LIBRARY}
        PUBLIC
            ${EVEREST_CORE_CMAKE_DIR}/assets/include
        PRIVATE
            ${CMAKE_CURRENT_BINARY_DIR}/generated/include 
    )
endfunction()

#
# out-of-tree interfaces/types/modules support
#
function(_ev_add_project)
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

    _ev_create_version_library(${EVEREST_PROJECT_NAME})

    install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/generated/version_information.txt
        DESTINATION "${CMAKE_INSTALL_DATADIR}/everest"
    )
endfunction()

macro(ev_add_project)
    ev_setup_cmake_variables_python_wheel()
    set(${PROJECT_NAME}_PYTHON_VENV_PATH "${CMAKE_BINARY_DIR}/venv" CACHE PATH "Path to python venv")

    ev_setup_python_executable(
        USE_PYTHON_VENV ${${PROJECT_NAME}_USE_PYTHON_VENV}
        PYTHON_VENV_PATH ${${PROJECT_NAME}_PYTHON_VENV_PATH}
    )

    setup_ev_cli()

    # FIXME (aw): resort to proper argument handling!
    if (${ARGC} EQUAL 2)
        _ev_add_project(${ARGV0} ${ARGV1})
    else()
        _ev_add_project()
    endif ()
endmacro()

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
