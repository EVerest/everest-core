ev_setup_cmake_variables_python_wheel()

ev_add_pip_package(
    NAME edm
    SOURCE_DIRECTORY dependency_manager
)

ev_is_python_venv_active(
    RESULT_VAR IS_PYTHON_VENV_ACTIVE
)

if(NOT ${IS_PYTHON_VENV_ACTIVE})
    message(WARNING "Python venv is not active. Please ensure that edm is available in your environment.")
else()
    get_target_property(SOURCE_DIRECTORY ev_pip_package_edm SOURCE_DIRECTORY)
    message(STATUS "Installing edm from: ${SOURCE_DIRECTORY}")
    ev_pip_install_local(
        PACKAGE_NAME "edm"
        PACKAGE_SOURCE_DIRECTORY "${SOURCE_DIRECTORY}"
    )
    unset(EVEREST_DEPENDENCY_MANAGER CACHE)
    find_program(EVEREST_DEPENDENCY_MANAGER edm HINTS ${EV_ACTIVATE_PYTHON_VENV_PATH_TO_VENV}/bin REQUIRED)
    message(STATUS "Using edm from: ${EDM}")
endif()
