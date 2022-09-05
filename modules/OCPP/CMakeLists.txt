#
# AUTO GENERATED - MARKED REGIONS WILL BE KEPT
# template version 3
#

# module setup:
#   - ${MODULE_NAME}: module name
ev_setup_cpp_module()

# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1
# insert your custom targets and additional config variables here
find_package(OpenSSL REQUIRED)

target_link_libraries(${MODULE_NAME}
    PRIVATE
        everest::ocpp
        OpenSSL::SSL
        OpenSSL::Crypto
)
# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1

target_sources(${MODULE_NAME}
    PRIVATE
        "main/ocpp_1_6_charge_pointImpl.cpp"
        "auth_validator/auth_token_validatorImpl.cpp"
        "auth_provider/auth_token_providerImpl.cpp"
)

# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
install(
    PROGRAMS
        constants.env
        diagnostics_uploader.sh
        firmware_updater.sh
        signed_firmware_downloader.sh
        signed_firmware_installer.sh
    DESTINATION "${EVEREST_MODULE_INSTALL_PREFIX}/${MODULE_NAME}"
)
# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
