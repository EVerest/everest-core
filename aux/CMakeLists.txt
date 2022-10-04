set(OCPP1_6_PROFILE_SCHEMAS_DIR "profile_schemas/ocpp1_6/")
set(USER_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/user_config/user_config.json")
set(SQL_INIT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/init.sql")
set(CERTS_DIR "certs/")

list(APPEND OCPP1_6_PROFILE_SCHEMAS
     Config.json
     Core.json
     FirmwareManagement.json
     Internal.json
     LocalAuthListManagement.json
     Reservation.json
     SmartCharging.json
     Security.json
)

list(TRANSFORM OCPP1_6_PROFILE_SCHEMAS
     PREPEND ${OCPP1_6_PROFILE_SCHEMAS_DIR})

install(
     FILES ${OCPP1_6_PROFILE_SCHEMAS}
     DESTINATION ${CMAKE_INSTALL_DATADIR}/everest/ocpp/profile_schemas/ocpp1_6
)

list(APPEND CONFIGS
     config.json
     config-docker.json
     config-docker-tls.json
     logging.ini
)

install(
     FILES ${CONFIGS}
     DESTINATION ${CMAKE_INSTALL_DATADIR}/everest/ocpp
)

install(
     DIRECTORY ${CERTS_DIR}
     DESTINATION ${CMAKE_INSTALL_DATADIR}/everest/ocpp/certs)

install(FILES ${SQL_INIT_FILE} DESTINATION ${CMAKE_INSTALL_DATADIR}/everest/ocpp)

if(EXISTS ${USER_CONFIG_FILE})
     install(
          FILES ${USER_CONFIG_FILE}
          DESTINATION ${CMAKE_INSTALL_DATADIR}/everest/ocpp)
endif()
