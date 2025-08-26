ev_add_module(GenericPowermeter)
ev_add_module(IsabellenhuetteIemDcr)
ev_add_module(LemDCBM400600)

if(${EVEREST_ENABLE_RS_SUPPORT})
    ev_add_module(RsIskraMeter)
endif()
