ev_add_module(NxpNfcFrontendTokenProvider)
ev_add_module(PN532TokenProvider)

if(EVEREST_DEPENDENCY_ENABLED_LIBNFC_NCI)
    ev_add_module(PN7160TokenProvider)
endif()
