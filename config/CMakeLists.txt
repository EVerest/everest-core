generate_config_run_script(CONFIG sil)
generate_config_run_script(CONFIG sil-two-evse)
generate_config_run_script(CONFIG sil-ocpp)
generate_config_run_script(CONFIG sil-ocpp201)
generate_config_run_script(CONFIG sil-dc)
generate_config_run_script(CONFIG sil-dc-sae-v2g)
generate_config_run_script(CONFIG sil-dc-sae-v2h)
generate_config_run_script(CONFIG sil-two-evse-dc)
generate_config_run_script(CONFIG sil-energy-management)
generate_config_run_script(CONFIG sil-gen-pm)
generate_config_run_script(CONFIG sil-ocpp)
generate_config_run_script(CONFIG sil-ocpp-custom-extension)
generate_config_run_script(CONFIG sil-ocpp-pnc)
generate_config_run_script(CONFIG sil-ocpp201-pnc)
generate_config_run_script(CONFIG example)
generate_config_run_script(CONFIG dash-20-sil)

# install configs
install(
    DIRECTORY "."
    DESTINATION "${CMAKE_INSTALL_SYSCONFDIR}/everest"
    FILES_MATCHING PATTERN "*.yaml"
)

# install certificates
install(
    DIRECTORY "certs"
    DESTINATION "${CMAKE_INSTALL_SYSCONFDIR}/everest"
    FILES_MATCHING PATTERN "*.pem" PATTERN "*.key" PATTERN "*.der" PATTERN "*.txt" PATTERN "*.jks" PATTERN "*.p12" 
)

install(
    FILES "${EVEREST_CONFIG_ASSET_DIR}/logging.ini"
    DESTINATION "${CMAKE_INSTALL_SYSCONFDIR}/everest"
    RENAME "default_logging.cfg"
)

add_subdirectory(nodered)
