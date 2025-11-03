set(OCPP1_6_PROFILE_SCHEMAS_DIR "profile_schemas/ocpp1_6/")

list(APPEND OCPP1_6_PROFILE_SCHEMAS
     Config.json
     Core.json
     Internal.json
     LocalAuthListManagement.json
     Reservation.json
     SmartCharging.json
)

list(TRANSFORM OCPP1_6_PROFILE_SCHEMAS
     PREPEND ${OCPP1_6_PROFILE_SCHEMAS_DIR})

install(FILES ${OCPP1_6_PROFILE_SCHEMAS}
        DESTINATION ocpp/profile_schemas/ocpp1_6)

list(APPEND CONFIGS
     config.json
     config-docker.json
     config-docker-tls.json
     logging.ini
)
install(FILES ${CONFIGS}
        DESTINATION ocpp)
