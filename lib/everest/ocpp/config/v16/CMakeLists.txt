set(OCPP1_6_PROFILE_SCHEMAS_DIR "profile_schemas/")
set(USER_CONFIG_FILE "user_config/user_config.json")
set(CERTS_DIR "../certs/")

# find all migration files, store only the filenames by showing relative to the folder
set(MIGRATION_FILES_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/core_migrations")

include(../CollectMigrationFiles.cmake)

collect_migration_files(
     LOCATION ${MIGRATION_FILES_LOCATION}
     INSTALL_DESTINATION ${CMAKE_INSTALL_DATADIR}/everest/modules/OCPP/core_migrations
     )

set(OCPP1_6_CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}" PARENT_SCOPE)
set(MIGRATION_FILE_VERSION_V16 ${TARGET_MIGRATION_FILE_VERSION} PARENT_SCOPE)
set(MIGRATION_FILES_SOURCE_DIR_V16 ${MIGRATION_FILES_LOCATION} PARENT_SCOPE)


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
     CostAndPrice.json
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
     config-full.json
     config-docker-tls.json
     ../logging.ini
)

install(
     FILES ${CONFIGS}
     DESTINATION ${CMAKE_INSTALL_DATADIR}/everest/modules/OCPP
)

if(EXISTS ${USER_CONFIG_FILE})
     install(
          FILES ${USER_CONFIG_FILE}
          DESTINATION ${CMAKE_INSTALL_DATADIR}/everest/modules/OCPP)
endif()
