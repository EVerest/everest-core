ev_create_pip_install_targets(
    PACKAGE_NAME
        everestpy
    DIST_DEPENDS
        everestpy_ln_dist
    LOCAL_DEPENDS
        everestpy_ln_local
)

ev_create_python_wheel_targets(
    PACKAGE_NAME
        everestpy
    DEPENDS
        everestpy_ln_dist
)

add_subdirectory(src/everest)
