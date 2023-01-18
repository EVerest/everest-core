generate_config_run_script(CONFIG sil)
generate_config_run_script(CONFIG sil-two-evse)
generate_config_run_script(CONFIG sil-ocpp)
generate_config_run_script(CONFIG sil-dc)
generate_config_run_script(CONFIG sil-two-evse-dc)
generate_config_run_script(CONFIG hil)

# install configs
install(
    DIRECTORY "."
    DESTINATION "${CMAKE_INSTALL_SYSCONFDIR}/everest"
    FILES_MATCHING PATTERN "*.yaml"
)

install(
    FILES "${EVEREST_CONFIG_ASSET_DIR}/logging.ini"
    DESTINATION "${CMAKE_INSTALL_SYSCONFDIR}/everest"
    RENAME "default_logging.cfg"
)

add_subdirectory(nodered)
