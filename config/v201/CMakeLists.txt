set(OCPP2_0_1_PROFILE_SCHEMAS_DIR "component_schemas/")

list(APPEND OCPP2_0_1_PROFILE_SCHEMAS
     AlignedDataCtrlr.json
     AuthCacheCtrlr.json
     AuthCtrlr.json
     CHAdeMOCtrlr.json
     CPPWMController.json
     ChargingStation.json
     ClockCtrlr.json
     Config.json
     ConnectedEV.json
     Connector.json
     Controller.json
     CustomizationCtrlr.json
     DeviceDataCtrlr.json
     DisplayMessageCtrlr.json
     DistributionPanel.json
     EVSE.json
     FiscalMetering.json
     ISO15118Ctrlr.json
     InternalCtrlr.json
     LocalAuthListCtrlr.json
     LocalEnergyStorage.json
     MonitoringCtrlr.json
     OCPPCommCtrlr.json
     ReservationCtrlr.json
     SampledDataCtrlr.json
     SecurityCtrlr.json
     SmartChargingCtrlr.json
     TariffCostCtrlr.json
     TokenReader.json
     TxCtrlr.json
)

list(TRANSFORM OCPP2_0_1_PROFILE_SCHEMAS
     PREPEND ${OCPP2_0_1_PROFILE_SCHEMAS_DIR})

install(
     FILES ${OCPP2_0_1_PROFILE_SCHEMAS}
     DESTINATION ${CMAKE_INSTALL_DATADIR}/everest/ocpp201/component_schemas
)

list(APPEND CONFIGS
     config.json
     ../logging.ini
)

install(
     FILES ${CONFIGS}
     DESTINATION ${CMAKE_INSTALL_DATADIR}/everest/ocpp201
)
