ev_add_module(API)
ev_add_module(Auth)
ev_add_module(EnergyManager)
ev_add_module(EnergyNode)
ev_add_module(EvAPI)
ev_add_module(EvManager)
ev_add_module(ErrorHistory)
ev_add_module(Evse15118D20)
ev_add_module(EvseManager)
ev_add_module(EvseSecurity)
ev_add_module(EvseSlac)
ev_add_module(EvseV2G)
ev_add_module(IsoMux)
ev_add_module(EvSlac)
ev_add_module(GenericPowermeter)
ev_add_module(LemDCBM400600)
ev_add_module(OCPP)
ev_add_module(OCPP201)
ev_add_module(PacketSniffer)
ev_add_module(PersistentStore)
ev_add_module(PN532TokenProvider)
ev_add_module(PN7160TokenProvider)
ev_add_module(PyEvJosev)
ev_add_module(Setup)
ev_add_module(SerialCommHub)
ev_add_module(StaticISO15118VASProvider)
ev_add_module(Store)
ev_add_module(System)
ev_add_module(YetiDriver)
ev_add_module(YetiEvDriver)
ev_add_module(DummyV2G)
ev_add_module(MicroMegaWattBSP)
ev_add_module(DPM1000)
ev_add_module(OCPPExtensionExample)
ev_add_module(DummyBankSessionTokenProvider)
ev_add_module(DummyTokenValidator)
ev_add_module(DummyTokenProvider)
ev_add_module(DummyTokenProviderManual)
ev_add_module(PhyVersoBSP)
ev_add_module(IsabellenhuetteIemDcr)
ev_add_module(AdAcEvse22KwzKitBSP)

add_subdirectory(examples)
add_subdirectory(simulation)

if(${EVEREST_ENABLE_RS_SUPPORT})
    ev_add_module(RsIskraMeter)
    ev_add_module(RsPaymentTerminal)
    add_subdirectory(rust_examples)
endif()
