macro(ev_create_python_venv)
    cmake_parse_arguments(
        "EV_CREATE_PYTHON_VENV"
        "INCLUDE_SYSTEM_SITE_PACKAGES"
        "PATH_TO_VENV"
        ""
        ${ARGN}
    )
    ev_is_directory_python_venv(
        DIRECTORY "${EV_CREATE_PYTHON_VENV_PATH_TO_VENV}"
        RESULT_VAR "EV_CREATE_PYTHON_VENV_IS_DIRECTORY_PYTHON_VENV"
    )
    if(${EV_CREATE_PYTHON_VENV_IS_DIRECTORY_PYTHON_VENV})
        message(FATAL_ERROR "Directory is already a python venv: ${EV_CREATE_PYTHON_VENV_PATH_TO_VENV}")
    endif()
    find_package(Python3
        REQUIRED
        COMPONENTS
            Interpreter
            Development
    )
    set(EV_CREATE_PYTHON_VENV_SYSTEM_SITE_PACKAGES_FLAG "")
    if(${EV_CREATE_PYTHON_VENV_INCLUDE_SYSTEM_SITE_PACKAGES})
        set(EV_CREATE_PYTHON_VENV_SYSTEM_SITE_PACKAGES_FLAG "--system-site-packages")
    endif()
    execute_process(
        COMMAND
            ${Python3_EXECUTABLE} -m venv ${EV_CREATE_PYTHON_VENV_SYSTEM_SITE_PACKAGES_FLAG} ${EV_CREATE_PYTHON_VENV_PATH_TO_VENV}
        RESULT_VARIABLE EV_CREATE_PYTHON_VENV_RESULT
    )
    if(${EV_CREATE_PYTHON_VENV_RESULT} AND NOT ${EV_CREATE_PYTHON_VENV_RESULT} EQUAL 0)
        message(FATAL_ERROR "Could not create python venv: ${EV_CREATE_PYTHON_VENV_PATH_TO_VENV}")
    endif()
    message(STATUS "Created python venv: ${EV_CREATE_PYTHON_VENV_PATH_TO_VENV}")
endmacro()

macro(ev_activate_python_venv)
    cmake_parse_arguments(
        "EV_ACTIVATE_PYTHON_VENV"
        ""
        "PATH_TO_VENV"
        ""
        ${ARGN}
    )
    ev_is_directory_python_venv(
        DIRECTORY "${EV_ACTIVATE_PYTHON_VENV_PATH_TO_VENV}"
        RESULT_VAR "EV_CREATE_PYTHON_VENV_IS_DIRECTORY_PYTHON_VENV"
    )
    if(NOT ${EV_CREATE_PYTHON_VENV_IS_DIRECTORY_PYTHON_VENV})
        message(FATAL_ERROR "Directory is not a python venv: ${EV_ACTIVATE_PYTHON_VENV_PATH_TO_VENV}")
    endif()
    set(ENV{VIRTUAL_ENV} "${EV_ACTIVATE_PYTHON_VENV_PATH_TO_VENV}")
    set(Python3_FIND_VIRTUALENV ONLY)
    unset(Python3_EXECUTABLE)
    find_package(Python3
        REQUIRED
        COMPONENTS
            Interpreter
            Development
    )
    message(STATUS "Activated python venv: Python3_EXECUTABLE: ${Python3_EXECUTABLE}")
endmacro()

macro(ev_deactivate_python_venv)
    unset(ENV{VIRTUAL_ENV})
    unset(Python3_EXECUTABLE)
    set(Python3_FIND_VIRTUALENV STANDARD)
    find_package(Python3
        REQUIRED
        COMPONENTS
            Interpreter
            Development
    )
    message(STATUS "Deactivated python venv: Python3_EXECUTABLE: ${Python3_EXECUTABLE}")
endmacro()

function(ev_is_directory_python_venv)
    cmake_parse_arguments(
        "EV_IS_DIRECTORY_PYTHON_VENV"
        ""
        "DIRECTORY;RESULT_VAR"
        ""
        ${ARGN}
    )
    message(DEBUG "Checking if directory is a python venv: ${EV_IS_DIRECTORY_PYTHON_VENV_DIRECTORY}")
    if(EXISTS "${EV_IS_DIRECTORY_PYTHON_VENV_DIRECTORY}/pyvenv.cfg")
        message(DEBUG "Directory is a python venv: ${EV_IS_DIRECTORY_PYTHON_VENV_DIRECTORY}")
        set(${EV_IS_DIRECTORY_PYTHON_VENV_RESULT_VAR} TRUE PARENT_SCOPE)
        return()
    else()
        message(DEBUG "Directory is not a python venv: ${EV_IS_DIRECTORY_PYTHON_VENV_DIRECTORY}")
        set(${EV_IS_DIRECTORY_PYTHON_VENV_RESULT_VAR} FALSE PARENT_SCOPE)
        return()
    endif()
endfunction()

function(ev_is_python_venv_active)
    cmake_parse_arguments(
        "EV_IS_PYTHON_VENV_ACTIVE"
        ""
        "RESULT_VAR"
        ""
        ${ARGN}
    )
    if(DEFINED ENV{VIRTUAL_ENV})
        message(DEBUG "Python venv is active: $ENV{VIRTUAL_ENV}")
        set(${EV_IS_PYTHON_VENV_ACTIVE_RESULT_VAR} TRUE PARENT_SCOPE)
        return()
    else()
        message(DEBUG "Python venv is not active")
        set(${EV_IS_PYTHON_VENV_ACTIVE_RESULT_VAR} FALSE PARENT_SCOPE)
        return()
    endif()
endfunction()

macro(ev_setup_python_executable)
    cmake_parse_arguments(
        "EV_SETUP_PYTHON_EXECUTABLE"
        ""
        "USE_PYTHON_VENV;PYTHON_VENV_PATH"
        ""
        ${ARGN}
    )
    if(NOT ${EV_SETUP_PYTHON_EXECUTABLE_USE_PYTHON_VENV})
        find_package(Python3
            REQUIRED
            COMPONENTS
                Interpreter
        )
    else()
        message(STATUS "Using python venv: ${EV_SETUP_PYTHON_EXECUTABLE_PYTHON_VENV_PATH}")
        ev_is_directory_python_venv(
            DIRECTORY ${EV_SETUP_PYTHON_EXECUTABLE_PYTHON_VENV_PATH}
            RESULT_VAR IS_DIRECTORY_PYTHON_VENV
        )
        if(NOT ${IS_DIRECTORY_PYTHON_VENV})
            ev_create_python_venv(
                PATH_TO_VENV
                    "${EV_SETUP_PYTHON_EXECUTABLE_PYTHON_VENV_PATH}"
                INCLUDE_SYSTEM_SITE_PACKAGES
            )
        endif()
        ev_activate_python_venv(PATH_TO_VENV "${EV_SETUP_PYTHON_EXECUTABLE_PYTHON_VENV_PATH}")
    endif()
    message(STATUS "Set up python executable with Python3_EXECUTABLE=${Python3_EXECUTABLE}")
endmacro()
