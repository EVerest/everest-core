# Internal macro to find the sphinx-build executable.
macro(_find_sphinx_build)
    find_program(
        _SPHINX_BUILD_EXECUTABLE
        NAMES 
            "${Python3_EXECUTABLE} -m sphinx.cmd.build"
            "sphinx-build"
        DOC "Path to the sphinx-build executable"
        OPTIONAL
        PATHS
            $ENV{VIRTUAL_ENV}/bin
    )
endmacro()

# Internal macro to find sphinx-build, and if not found, try to install it in an active python venv.
macro(_find_and_fix_sphinx_build)
    _find_sphinx_build()

    if("${_SPHINX_BUILD_EXECUTABLE}" STREQUAL "_SPHINX_BUILD_EXECUTABLE-NOTFOUND")
        ev_is_python_venv_active(
            RESULT_VAR IS_PYTHON_VENV_ACTIVE
        )
        if(IS_PYTHON_VENV_ACTIVE)
            message(STATUS "sphinx-build executable not found in system, but python venv is active. Trying to use 'python3 -m pip install sphinx'.")
            execute_process(
                COMMAND ${Python3_EXECUTABLE} -m pip install sphinx
            )
            _find_sphinx_build()
        endif()
    endif()

    if("${_SPHINX_BUILD_EXECUTABLE}" STREQUAL "_SPHINX_BUILD_EXECUTABLE-NOTFOUND")
        message(FATAL_ERROR "sphinx-build executable not found. Please install Sphinx. You can install it via pip: pip install sphinx")
    endif()

    message(STATUS "Found sphinx-build: ${_SPHINX_BUILD_EXECUTABLE}")
endmacro()

# Internal macro to set up the trailbook environment.
macro(_setup_trailbook)
    if(NOT _TRAILBOOK_SETUP_DONE)
        _find_and_fix_sphinx_build()

        set(_TRAILBOOK_SETUP_DONE TRUE)
    endif()
endmacro()