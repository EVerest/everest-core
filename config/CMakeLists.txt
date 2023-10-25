add_subdirectory(nodered)

generate_config_run_script(CONFIG sil)
generate_config_run_script(CONFIG sil-two-evse)
generate_config_run_script(CONFIG sil-ocpp)
generate_config_run_script(CONFIG sil-ocpp201)
generate_config_run_script(CONFIG sil-dc)
generate_config_run_script(CONFIG sil-two-evse-dc)
generate_config_run_script(CONFIG sil-energy-management)
generate_config_run_script(CONFIG hil)
generate_config_run_script(CONFIG sil-gen-pm)
generate_config_run_script(CONFIG sil-ocpp)
generate_config_run_script(CONFIG sil-ocpp-custom-extension)
generate_config_run_script(CONFIG sil-ocpp-pnc)

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

# generate and install certificates
set(PKI_GENERATOR_SCRIPT ${PROJECT_SOURCE_DIR}/script/simple/generate_certificates.py)
set(PKI_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/certs)
set(PKI_GENERATION_STAMP_FILE ${PKI_OUTPUT_DIR}/.stamp)

add_custom_command(OUTPUT ${PKI_GENERATION_STAMP_FILE}
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${PKI_OUTPUT_DIR}
    COMMAND ${PKI_GENERATOR_SCRIPT} --output-dir ${PKI_OUTPUT_DIR}
    COMMAND ${CMAKE_COMMAND} -E touch ${PKI_GENERATION_STAMP_FILE}
    COMMENT "Generating V2G PKI"
    DEPENDS ${PKI_GENERATOR_SCRIPT}
)

add_custom_target(everest_core_generate_v2g_pki ALL
    DEPENDS ${PKI_GENERATION_STAMP_FILE}
)

install(
    DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/certs"
    DESTINATION "${CMAKE_INSTALL_SYSCONFDIR}/everest"
    FILES_MATCHING PATTERN "*.pem" PATTERN "*.key" PATTERN "*.der" PATTERN "*.txt" PATTERN "*.jks" PATTERN "*.p12" 
)

