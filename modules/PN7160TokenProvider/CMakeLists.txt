#
# AUTO GENERATED - MARKED REGIONS WILL BE KEPT
# template version 3
#

# module setup:
#   - ${MODULE_NAME}: module name
ev_setup_cpp_module()

# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1

target_include_directories(${MODULE_NAME}
PRIVATE
    "common"
)

target_sources(${MODULE_NAME}
    PRIVATE
        "main/libnfc_handler.cpp"
)

target_link_libraries(${MODULE_NAME}
    PRIVATE
        "libnfc_nci"
)
# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1

target_sources(${MODULE_NAME}
    PRIVATE
        "main/auth_token_providerImpl.cpp"
)

# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1

get_target_property(LIBNFCNCI_CONFIG_INSTALL_PATH libnfc_nci::libnfc_nci LIBNFCNCI_CONFIG_INSTALL_PATH)
install(FILES
            "libnfc-nci_config/libnfc-nxp.conf"
            "libnfc-nci_config/libnfc-nci.conf"
        DESTINATION
	    "${LIBNFCNCI_CONFIG_INSTALL_PATH}")

# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
