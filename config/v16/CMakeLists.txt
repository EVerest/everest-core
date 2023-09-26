set(OCPP1_6_PROFILE_SCHEMAS_DIR "profile_schemas/")
set(USER_CONFIG_FILE "user_config/user_config.json")
set(SQL_INIT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/init.sql")
set(CERTS_DIR "../certs/")

list(APPEND OCPP1_6_PROFILE_SCHEMAS
     Config.json
     Core.json
     FirmwareManagement.json
     Internal.json
     LocalAuthListManagement.json
     Reservation.json
     SmartCharging.json
     Security.json
     PnC.json
)

list(TRANSFORM OCPP1_6_PROFILE_SCHEMAS
     PREPEND ${OCPP1_6_PROFILE_SCHEMAS_DIR})

install(
     FILES ${OCPP1_6_PROFILE_SCHEMAS}
     DESTINATION ${CMAKE_INSTALL_DATADIR}/everest/modules/OCPP/profile_schemas
)

list(APPEND CONFIGS
     config.json
     config-docker.json
     config-docker-tls.json
     ../logging.ini
)

install(
     FILES ${CONFIGS}
     DESTINATION ${CMAKE_INSTALL_DATADIR}/everest/modules/OCPP
)

install(FILES ${SQL_INIT_FILE} DESTINATION ${CMAKE_INSTALL_DATADIR}/everest/modules/OCPP)

if(EXISTS ${USER_CONFIG_FILE})
     install(
          FILES ${USER_CONFIG_FILE}
          DESTINATION ${CMAKE_INSTALL_DATADIR}/everest/modules/OCPP)
endif()
