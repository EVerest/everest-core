install(
    FILES
        interface.yaml
        config.yaml
        manifest.yaml
        type.yaml
        error-declaration-list.yaml
        error-object.yaml
    DESTINATION "${CMAKE_INSTALL_DATADIR}/everest/schemas"
)
