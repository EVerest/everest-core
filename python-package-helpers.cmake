function (ev_create_pip_install_dist_target)
    set(oneValueArgs
        PACKAGE_NAME
        PACKAGE_SOURCE_DIRECTORY
    )
    set(multiValueArgs
        DEPENDS
    )
    cmake_parse_arguments(
        "EV_CREATE_PIP_INSTALL_DIST_TARGET"
        ""
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )
    if ("${EV_CREATE_PIP_INSTALL_DIST_TARGET_PACKAGE_NAME}" STREQUAL "")
        message(FATAL_ERROR "PACKAGE_NAME is required")
    endif()
    if ("${EV_CREATE_PIP_INSTALL_DIST_TARGET_PACKAGE_SOURCE_DIRECTORY}" STREQUAL "")
        set(EV_CREATE_PIP_INSTALL_DIST_TARGET_PACKAGE_SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    set(CHECK_DONE_FILE "${CMAKE_BINARY_DIR}/${EV_CREATE_PIP_INSTALL_DIST_TARGET_PACKAGE_NAME}_pip_install_dist_installed")
    add_custom_command(
        OUTPUT
            "${CHECK_DONE_FILE}"
        COMMENT
            "Installing ${EV_CREATE_PIP_INSTALL_DIST_TARGET_PACKAGE_NAME} from distribution"
        WORKING_DIRECTORY
            ${EV_CREATE_PIP_INSTALL_DIST_TARGET_PACKAGE_SOURCE_DIRECTORY}
        # Remove build dir from pip
        COMMAND
            ${CMAKE_COMMAND} -E remove_directory build
        COMMAND
            ${Python3_EXECUTABLE} -m pip install --force-reinstall .
        COMMAND
            ${CMAKE_COMMAND} -E touch "${CHECK_DONE_FILE}"
    )

    add_custom_target(${EV_CREATE_PIP_INSTALL_DIST_TARGET_PACKAGE_NAME}_pip_install_dist
        DEPENDS
            "${CHECK_DONE_FILE}"
        DEPENDS
            ${EV_CREATE_PIP_INSTALL_DIST_TARGET_DEPENDS}
    )
endfunction()

function (ev_create_pip_install_local_target)
    set(oneValueArgs
        PACKAGE_NAME
        PACKAGE_SOURCE_DIRECTORY
    )
    set(multiValueArgs
        DEPENDS
    )
    cmake_parse_arguments(
        "EV_CREATE_PIP_INSTALL_LOCAL_TARGET"
        ""
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )
    if ("${EV_CREATE_PIP_INSTALL_LOCAL_TARGET_PACKAGE_NAME}" STREQUAL "")
        message(FATAL_ERROR "PACKAGE_NAME is required")
    endif()
    if ("${EV_CREATE_PIP_INSTALL_LOCAL_TARGET_PACKAGE_SOURCE_DIRECTORY}" STREQUAL "")
        set(EV_CREATE_PIP_INSTALL_LOCAL_TARGET_PACKAGE_SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    add_custom_target(${EV_CREATE_PIP_INSTALL_LOCAL_TARGET_PACKAGE_NAME}_pip_install_local
        # Remove build dir from pip
        COMMAND
            ${CMAKE_COMMAND} -E remove_directory build
        COMMAND
            ${Python3_EXECUTABLE} -m pip install --force-reinstall -e .
        WORKING_DIRECTORY
            ${EV_CREATE_PIP_INSTALL_LOCAL_TARGET_PACKAGE_SOURCE_DIRECTORY}
        DEPENDS
            ${EV_CREATE_PIP_INSTALL_LOCAL_TARGET_DEPENDS}
        COMMENT
            "Installing ${EV_CREATE_PIP_INSTALL_LOCAL_TARGET_PACKAGE_NAME} via user-mode from build"
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
        "EV_CREATE_PIP_INSTALL_TARGETS"
        ""
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )
    ev_create_pip_install_dist_target(
        PACKAGE_NAME ${EV_CREATE_PIP_INSTALL_TARGETS_PACKAGE_NAME}
        PACKAGE_SOURCE_DIRECTORY ${EV_CREATE_PIP_INSTALL_TARGETS_PACKAGE_SOURCE_DIRECTORY}
        DEPENDS ${EV_CREATE_PIP_INSTALL_TARGETS_DIST_DEPENDS}
    )
    ev_create_pip_install_local_target(
        PACKAGE_NAME ${EV_CREATE_PIP_INSTALL_TARGETS_PACKAGE_NAME}
        PACKAGE_SOURCE_DIRECTORY ${EV_CREATE_PIP_INSTALL_TARGETS_PACKAGE_SOURCE_DIRECTORY}
        DEPENDS ${EV_CREATE_PIP_INSTALL_TARGETS_LOCAL_DEPENDS}
    )
endfunction()

function (ev_create_python_wheel_targets)
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
    if ("${EV_CREATE_PYTHON_WHEEL_TARGETS_PACKAGE_NAME}" STREQUAL "")
        message(FATAL_ERROR "PACKAGE_NAME is required")
    endif()
    if ("${EV_CREATE_PYTHON_WHEEL_TARGETS_PACKAGE_SOURCE_DIRECTORY}" STREQUAL "")
        set(EV_CREATE_PYTHON_WHEEL_TARGETS_PACKAGE_SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    endif()
    if (NOT DEFINED ${EV_CREATE_PYTHON_WHEEL_TARGETS_INSTALL_PREFIX})
        if ("${${PROJECT_NAME}_WHEEL_INSTALL_PREFIX}" STREQUAL "")
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

    add_custom_command(
        OUTPUT
            "${CHECK_DONE_FILE}"
        # Remove build dir from pip
        COMMAND
            ${CMAKE_COMMAND} -E remove_directory build
        COMMAND
            ${Python3_EXECUTABLE} -m build --wheel --outdir ${WHEEL_OUTDIR} .
        COMMAND
            ${CMAKE_COMMAND} -E touch "${CHECK_DONE_FILE}"
        WORKING_DIRECTORY
            ${EV_CREATE_PYTHON_WHEEL_TARGETS_PACKAGE_SOURCE_DIRECTORY}
        DEPENDS
            ${EV_CREATE_PYTHON_WHEEL_TARGETS_DEPENDS}
        COMMENT
            "Building wheel for ${EV_CREATE_PYTHON_WHEEL_TARGETS_PACKAGE_NAME}"
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
            "Copy wheel for ${EV_CREATE_PYTHON_WHEEL_TARGETS_PACKAGE_NAME} to ${EV_CREATE_PYTHON_WHEEL_TARGETS_INSTALL_PREFIX}"
    )
endfunction()

macro(ev_setup_cmake_variables_python_wheel)
    set(${PROJECT_NAME}_WHEEL_INSTALL_PREFIX "" CACHE PATH "Path to install python wheels to")
    if (${PROJECT_NAME}_WHEEL_INSTALL_PREFIX STREQUAL "")
        if(${WHEEL_INSTALL_PREFIX})
            set(${PROJECT_NAME}_DEFAULT_WHEEL_INSTALL_PREFIX "${WHEEL_INSTALL_PREFIX}")
            message(STATUS "${PROJET_NAME}_WHEEL_INSTALL_PREFIX not set, using default: WHEEL_INSTALL_PREFIX=${${PROJECT_NAME}_DEFAULT_WHEEL_INSTALL_PREFIX}")
        else()
            set(${PROJECT_NAME}_DEFAULT_WHEEL_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/../dist-wheels")
            message(STATUS "${PROJECT_NAME}_WHEEL_INSTALL_PREFIX not set, using default: \${CMAKE_INSTALL_PREFIX}/../dist-wheels=${${PROJECT_NAME}_DEFAULT_WHEEL_INSTALL_PREFIX}")
        endif()
        set(${PROJECT_NAME}_WHEEL_INSTALL_PREFIX "${${PROJECT_NAME}_DEFAULT_WHEEL_INSTALL_PREFIX}" CACHE PATH "Path to install python wheels to" FORCE)
    endif()
    message(STATUS "${PROJECT_NAME}_WHEEL_INSTALL_PREFIX=${${PROJECT_NAME}_WHEEL_INSTALL_PREFIX}")
endmacro()
