add_custom_target(eebus_generate_and_install_certificates ALL)

eebus_create_cert(
    NAME evse
    OUT_DIR ${CMAKE_BINARY_DIR}/certs/eebus
    TARGET_NAME eebus_evse_cert
    OUT_FILES_VAR EEBUS_CERT_FILES
)
add_dependencies(eebus_generate_and_install_certificates eebus_evse_cert)

install(
    FILES
        ${EEBUS_CERT_FILES}
    DESTINATION
        ${CMAKE_INSTALL_SYSCONFDIR}/everest/certs/eebus
)
