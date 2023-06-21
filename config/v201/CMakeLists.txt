set(SQL_INIT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/init_core.sql")

list(APPEND CONFIGS
     config.json
     ../logging.ini
)

install(
     FILES ${CONFIGS}
     DESTINATION ${CMAKE_INSTALL_DATADIR}/everest/modules/OCPP201
)

install(FILES ${SQL_INIT_FILE} DESTINATION ${CMAKE_INSTALL_DATADIR}/everest/modules/OCPP201)