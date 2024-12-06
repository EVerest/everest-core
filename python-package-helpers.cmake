function(ev_add_pip_package)
    set(one_value_args
        NAME
        SOURCE_DIRECTORY
    )

    cmake_parse_arguments(
        "args"
        "" # no optional arguments
        "${one_value_args}"
        ""
        ${ARGN}
    )

    if("${args_NAME}" STREQUAL "")
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION}: NAME is required")
    endif()

    cmake_path(ABSOLUTE_PATH args_SOURCE_DIRECTORY NORMALIZE)

    set(TARGET_NAME "ev_pip_package_${args_NAME}")
    add_custom_target(${TARGET_NAME})

    set_target_properties(${TARGET_NAME}
        PROPERTIES
            SOURCE_DIRECTORY "${args_SOURCE_DIRECTORY}"
    )
endfunction()

function(ev_install_pip_package)
    set(options
        LOCAL
        FORCE
    )

    set(one_value_args
        NAME
    )

    cmake_parse_arguments(
        "args"
        "${options}"
        "${one_value_args}"
        ""
        ${ARGN}
    )

    set(TARGET_NAME "ev_pip_package_${args_NAME}")

    if(NOT TARGET ${TARGET_NAME})
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION}: target ${TARGET_NAME} for package ${args_NAME} does not exist")
    endif()

    get_target_property(SOURCE_DIRECTORY ${TARGET_NAME} SOURCE_DIRECTORY)

    # NOTE (aw): probably we also want to directly forward arguments
    set(PIP_INSTALL_ARGS "")

    if(args_LOCAL)
        list(APPEND PIP_INSTALL_ARGS --user)
    endif()

    if(args_FORCE)
        list(APPEND PIP_INSTALL_ARGS --force-reinstall)
    endif()

    # FIXME (aw): check wether a check done file is necessary here
    # if so, it should be inside a separate function
    # discussion of update behavior is needed before
    execute_process(
        COMMAND
            ${Python3_EXECUTABLE} -m pip install ${PIP_INSTALL_ARGS} .
        WORKING_DIRECTORY
            ${SOURCE_DIRECTORY}
    )
endfunction()

function(ev_create_pip_install_dist_target)
    set(oneValueArgs
        PACKAGE_NAME
        PACKAGE_SOURCE_DIRECTORY
    )
    set(multiValueArgs
        DEPENDS
    )
    cmake_parse_arguments(
        "arg"
        ""
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    if("${arg_PACKAGE_NAME}" STREQUAL "")
        message(FATAL_ERROR "PACKAGE_NAME is required")
    endif()

    if("${arg_PACKAGE_SOURCE_DIRECTORY}" STREQUAL "")
        set(arg_PACKAGE_SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
        message(STATUS "ev_create_pip_install_dist_target: no PACKAGE_SOURCE_DIRECTORY provided, using: ${arg_PACKAGE_SOURCE_DIRECTORY}")
    endif()

    set(CHECK_DONE_FILE "${CMAKE_BINARY_DIR}/${arg_PACKAGE_NAME}_pip_install_dist_installed")
    add_custom_command(
        OUTPUT
            "${CHECK_DONE_FILE}"
        COMMENT
            "Installing ${arg_PACKAGE_NAME} from distribution"
        WORKING_DIRECTORY
            ${arg_PACKAGE_SOURCE_DIRECTORY}

        # Remove build dir from pip
        COMMAND
            ${CMAKE_COMMAND} -E remove_directory build
        COMMAND
            ${Python3_EXECUTABLE} -m pip install --force-reinstall .
        COMMAND
            ${CMAKE_COMMAND} -E touch "${CHECK_DONE_FILE}"
    )

    set(TARGET_NAME "${arg_PACKAGE_NAME}_pip_install_dist")
    add_custom_target(${TARGET_NAME}
        DEPENDS
        "${CHECK_DONE_FILE}"
        DEPENDS
        ${arg_DEPENDS}
    )
    set_target_properties(${TARGET_NAME}
        PROPERTIES
        PACKAGE_SOURCE_DIRECTORY "${arg_PACKAGE_SOURCE_DIRECTORY}"
    )
endfunction()

function(ev_create_pip_install_local_target)
    set(oneValueArgs
        PACKAGE_NAME
        PACKAGE_SOURCE_DIRECTORY
    )
    set(multiValueArgs
        DEPENDS
    )
    cmake_parse_arguments(
        "arg"
        ""
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    if("${arg_PACKAGE_NAME}" STREQUAL "")
        message(FATAL_ERROR "PACKAGE_NAME is required")
    endif()

    if("${arg_PACKAGE_SOURCE_DIRECTORY}" STREQUAL "")
        set(arg_PACKAGE_SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
        message(STATUS "ev_create_pip_install_local_target: no PACKAGE_SOURCE_DIRECTORY provided, using: ${arg_PACKAGE_SOURCE_DIRECTORY}")
    endif()

    set(TARGET_NAME "${arg_PACKAGE_NAME}_pip_install_local")
    add_custom_target(${TARGET_NAME}

        # Remove build dir from pip
        COMMAND
            ${CMAKE_COMMAND} -E remove_directory build
        COMMAND
            ${Python3_EXECUTABLE} -m pip install --force-reinstall -e .
        WORKING_DIRECTORY
            ${arg_PACKAGE_SOURCE_DIRECTORY}
        DEPENDS
            ${arg_DEPENDS}
        COMMENT
            "Installing ${arg_PACKAGE_NAME} via user-mode from build"
    )
    set_target_properties(${TARGET_NAME}
        PROPERTIES
        PACKAGE_SOURCE_DIRECTORY "${arg_PACKAGE_SOURCE_DIRECTORY}"
    )
endfunction()

function(ev_create_pip_install_targets)
    set(oneValueArgs
        PACKAGE_NAME
        PACKAGE_SOURCE_DIRECTORY
    )
    set(multiValueArgs
        DIST_DEPENDS
        LOCAL_DEPENDS
    )
    cmake_parse_arguments(
        "arg"
        ""
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )
    ev_create_pip_install_dist_target(
        PACKAGE_NAME ${arg_PACKAGE_NAME}
        PACKAGE_SOURCE_DIRECTORY ${arg_PACKAGE_SOURCE_DIRECTORY}
        DEPENDS ${arg_DIST_DEPENDS}
    )
    ev_create_pip_install_local_target(
        PACKAGE_NAME ${arg_PACKAGE_NAME}
        PACKAGE_SOURCE_DIRECTORY ${arg_PACKAGE_SOURCE_DIRECTORY}
        DEPENDS ${arg_LOCAL_DEPENDS}
    )
endfunction()

function(ev_create_python_wheel_targets)
    set(oneValueArgs
        PACKAGE_NAME
        PACKAGE_SOURCE_DIRECTORY
        INSTALL_PREFIX
    )
    set(multiValueArgs
        DEPENDS
    )
    cmake_parse_arguments(
        "EV_CREATE_PYTHON_WHEEL_TARGETS"
        ""
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    if("${EV_CREATE_PYTHON_WHEEL_TARGETS_PACKAGE_NAME}" STREQUAL "")
        message(FATAL_ERROR "PACKAGE_NAME is required")
    endif()

    if("${EV_CREATE_PYTHON_WHEEL_TARGETS_PACKAGE_SOURCE_DIRECTORY}" STREQUAL "")
        set(EV_CREATE_PYTHON_WHEEL_TARGETS_PACKAGE_SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    if(NOT DEFINED ${EV_CREATE_PYTHON_WHEEL_TARGETS_INSTALL_PREFIX})
        if("${${PROJECT_NAME}_WHEEL_INSTALL_PREFIX}" STREQUAL "")
            message(FATAL_ERROR
                "No install prefix for wheel specified, please set ${PROJECT_NAME}_WHEEL_INSTALL_PREFIX, use the INSTALL_PREFIX argument or use macro ev_setup_cmake_variables_python_wheel() to set a default value."
            )
        else()
            set(EV_CREATE_PYTHON_WHEEL_TARGETS_INSTALL_PREFIX ${${PROJECT_NAME}_WHEEL_INSTALL_PREFIX})
        endif()
    endif()

    set(WHEEL_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/dist_${EV_CREATE_PYTHON_WHEEL_TARGETS_PACKAGE_NAME})
    set(CHECK_DONE_FILE ${CMAKE_CURRENT_BINARY_DIR}/${EV_CREATE_PYTHON_WHEEL_TARGETS_PACKAGE_NAME}_build_wheel_done)

    add_custom_target(${EV_CREATE_PYTHON_WHEEL_TARGETS_PACKAGE_NAME}_build_wheel
        DEPENDS
            "${CHECK_DONE_FILE}"
    )

    set(USE_WHEELS "ON" CACHE STRING "Enable or disable the use of python wheels - if off switch to tar.gz format")
    message(STATUS "USE_WHEELS is set to: ${USE_WHEELS}")

    if(USE_WHEELS)
        set(PACKAGE_BUILD_COMMAND ${Python3_EXECUTABLE} -m build --wheel --outdir ${WHEEL_OUTDIR} .)
        set(PACKAGE_REMOVE_DIR_COMMAND ${CMAKE_COMMAND} -E rm -rf build src/*.egg-info)
    else()
        set(PACKAGE_BUILD_COMMAND ${Python3_EXECUTABLE} setup.py sdist --dist-dir ${WHEEL_OUTDIR})
        set(PACKAGE_REMOVE_DIR_COMMAND ${CMAKE_COMMAND} -E rm -rf src/*.egg-info)
    endif()

    add_custom_command(
        OUTPUT
        "${CHECK_DONE_FILE}"

        COMMAND
            ${PACKAGE_BUILD_COMMAND}
        COMMAND
            ${PACKAGE_REMOVE_DIR_COMMAND}
        COMMAND
            ${CMAKE_COMMAND} -E touch "${CHECK_DONE_FILE}"
        WORKING_DIRECTORY
            ${EV_CREATE_PYTHON_WHEEL_TARGETS_PACKAGE_SOURCE_DIRECTORY}
        DEPENDS
            ${EV_CREATE_PYTHON_WHEEL_TARGETS_DEPENDS}
        COMMENT
            "Building Python package for module ${EV_CREATE_PYTHON_WHEEL_TARGETS_PACKAGE_NAME}"
    )

    add_custom_target(${EV_CREATE_PYTHON_WHEEL_TARGETS_PACKAGE_NAME}_install_wheel
        COMMAND
            ${CMAKE_COMMAND} -E make_directory ${EV_CREATE_PYTHON_WHEEL_TARGETS_INSTALL_PREFIX}
        COMMAND
            ${CMAKE_COMMAND} -E copy_directory ${WHEEL_OUTDIR} ${EV_CREATE_PYTHON_WHEEL_TARGETS_INSTALL_PREFIX}/
        WORKING_DIRECTORY
            ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS
            ${EV_CREATE_PYTHON_WHEEL_TARGETS_PACKAGE_NAME}_build_wheel
        COMMENT
            "Copy Python package for module ${EV_CREATE_PYTHON_WHEEL_TARGETS_PACKAGE_NAME} to ${EV_CREATE_PYTHON_WHEEL_TARGETS_INSTALL_PREFIX}"
    )
endfunction()

macro(ev_setup_cmake_variables_python_wheel)
    set(${PROJECT_NAME}_WHEEL_INSTALL_PREFIX "" CACHE PATH "Path to install python package to")

    if(${PROJECT_NAME}_WHEEL_INSTALL_PREFIX STREQUAL "")
        if(NOT ${WHEEL_INSTALL_PREFIX} STREQUAL "")
            set(${PROJECT_NAME}_DEFAULT_WHEEL_INSTALL_PREFIX "${WHEEL_INSTALL_PREFIX}")
            message(STATUS "${PROJECT_NAME}_WHEEL_INSTALL_PREFIX not set, using: WHEEL_INSTALL_PREFIX=${${PROJECT_NAME}_DEFAULT_WHEEL_INSTALL_PREFIX}")
        else()
            set(${PROJECT_NAME}_DEFAULT_WHEEL_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/../dist-wheels")
            message(STATUS "${PROJECT_NAME}_WHEEL_INSTALL_PREFIX and WHEEL_INSTALL_PREFIX not set, using default: \${CMAKE_INSTALL_PREFIX}/../dist-wheels=${${PROJECT_NAME}_DEFAULT_WHEEL_INSTALL_PREFIX}")
        endif()

        set(${PROJECT_NAME}_WHEEL_INSTALL_PREFIX "${${PROJECT_NAME}_DEFAULT_WHEEL_INSTALL_PREFIX}" CACHE PATH "Path to install python package to" FORCE)
    endif()

    message(STATUS "${PROJECT_NAME}_WHEEL_INSTALL_PREFIX=${${PROJECT_NAME}_WHEEL_INSTALL_PREFIX}")
endmacro()

function(ev_pip_install_local)
    set(oneValueArgs
        PACKAGE_NAME
        PACKAGE_SOURCE_DIRECTORY
    )
    set(multiValueArgs
        DEPENDS
    )
    cmake_parse_arguments(
        "EV_PIP_INSTALL_LOCAL"
        ""
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    if("${EV_PIP_INSTALL_LOCAL_PACKAGE_NAME}" STREQUAL "")
        message(FATAL_ERROR "PACKAGE_NAME is required")
    endif()

    if("${EV_PIP_INSTALL_LOCAL_PACKAGE_SOURCE_DIRECTORY}" STREQUAL "")
        set(EV_PIP_INSTALL_LOCAL_PACKAGE_SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    execute_process(
        COMMAND
            ${Python3_EXECUTABLE} -c "from setuptools import setup; setup()" --version
        WORKING_DIRECTORY
            ${EV_PIP_INSTALL_LOCAL_PACKAGE_SOURCE_DIRECTORY}
        OUTPUT_VARIABLE
            EV_PIP_INSTALL_LOCAL_INSTALLED_PACKAGE_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    set(CHECK_DONE_FILE "${CMAKE_BINARY_DIR}/${EV_PIP_INSTALL_LOCAL_PACKAGE_NAME}_pip_install_local_installed_${EV_PIP_INSTALL_LOCAL_INSTALLED_PACKAGE_VERSION}")

    if(NOT EXISTS "${CHECK_DONE_FILE}")
        message(STATUS "${EV_PIP_INSTALL_LOCAL_PACKAGE_NAME} not found, installing.")
        execute_process(
            COMMAND
                ${Python3_EXECUTABLE} -m pip install --force-reinstall -e .
            WORKING_DIRECTORY
                ${EV_PIP_INSTALL_LOCAL_PACKAGE_SOURCE_DIRECTORY}
            RESULTS_VARIABLE EV_RESULTS
        )
        execute_process(
            COMMAND
                ${CMAKE_COMMAND} -E touch "${CHECK_DONE_FILE}"
            RESULTS_VARIABLE EV_RESULTS
        )
    endif()

    message(STATUS "Using ${EV_PIP_INSTALL_LOCAL_PACKAGE_NAME} from ${CMAKE_CURRENT_SOURCE_DIR} version: ${EV_PIP_INSTALL_LOCAL_INSTALLED_PACKAGE_VERSION}")
endfunction()
