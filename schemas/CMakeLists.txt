install(
    FILES
        interface.yaml
        config.yaml
        manifest.yaml
        type.yaml
    DESTINATION "${CMAKE_INSTALL_DATADIR}/everest/schemas"
)
