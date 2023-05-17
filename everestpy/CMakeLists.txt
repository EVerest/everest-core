add_subdirectory(src/everest)

add_custom_target(everestpy_pip_install_dist
    # remove build dir from pip
    COMMAND
        cmake -E remove_directory build
    COMMAND
        ${PYTHON_EXECUTABLE} -m pip install --force-reinstall .
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS
        everestpy_ln_dist
    COMMENT
        "Installing everestpy from distribution"
)

add_custom_target(everestpy_pip_install_local
    # remove build dir from pip
    COMMAND
        cmake -E remove_directory build
    COMMAND
        ${PYTHON_EXECUTABLE} -m pip install --force-reinstall -e .
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS
        everestpy_ln_local
    COMMENT
        "Installing everestpy via user-mode from build"
)
