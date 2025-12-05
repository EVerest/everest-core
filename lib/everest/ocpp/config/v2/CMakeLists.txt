# find all migration files, store only the filenames by showing relative to the folder
set(MIGRATION_FILES_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/core_migrations")
set(MIGRATION_FILES_DEVICE_MODEL_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/device_model_migrations")

include(../CollectMigrationFiles.cmake)

collect_migration_files(
     LOCATION ${MIGRATION_FILES_LOCATION}
     INSTALL_DESTINATION ${CMAKE_INSTALL_DATADIR}/everest/modules/OCPP201/core_migrations
     )

set(MIGRATION_FILE_VERSION_V2 ${TARGET_MIGRATION_FILE_VERSION} PARENT_SCOPE)

collect_migration_files(
     LOCATION ${MIGRATION_FILES_DEVICE_MODEL_LOCATION}
     INSTALL_DESTINATION ${CMAKE_INSTALL_DATADIR}/everest/modules/OCPP201/device_model_migrations
     )

set(MIGRATION_DEVICE_MODEL_FILE_VERSION_V2 ${TARGET_MIGRATION_FILE_VERSION} PARENT_SCOPE)
set(MIGRATION_FILES_SOURCE_DIR_V2 ${MIGRATION_FILES_LOCATION} PARENT_SCOPE)
set(MIGRATION_FILES_DEVICE_MODEL_SOURCE_DIR_V2 ${MIGRATION_FILES_DEVICE_MODEL_LOCATION} PARENT_SCOPE)

option(LIBOCPP_INSTALL_STANDARDIZED_COMPONENT_CONFIG "Install the standardized components configs for OCPP2.0.1 and OCPP2.1" ON)
option(LIBOCPP_INSTALL_CUSTOM_COMPONENT_CONFIG "Install the custom component configs for OCPP2.0.1 and OCPP2.1" OFF)

list(APPEND CONFIGS
     ../logging.ini
)

install(
     FILES ${CONFIGS}
     DESTINATION ${CMAKE_INSTALL_DATADIR}/everest/modules/OCPP201
)

set(LIBOCPP_STANDARDIZED_COMPONENT_CONFIG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/component_config/standardized" CACHE PATH "Path to standardized OCPP component configs")
set(LIBOCPP_CUSTOM_COMPONENT_CONFIG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/component_config/custom" CACHE PATH "Path to custom OCPP component configs")

if (LIBOCPP_INSTALL_STANDARDIZED_COMPONENT_CONFIG)
     message(STATUS "Using ocpp v2 standardized component config file path: ${LIBOCPP_STANDARDIZED_COMPONENT_CONFIG_PATH}")
     install(DIRECTORY ${LIBOCPP_STANDARDIZED_COMPONENT_CONFIG_PATH}/ DESTINATION ${CMAKE_INSTALL_DATADIR}/everest/modules/OCPP201/component_config/standardized)
endif()

if (LIBOCPP_INSTALL_CUSTOM_COMPONENT_CONFIG)
     message(STATUS "Using ocpp v2 custom component config file path: ${LIBOCPP_CUSTOM_COMPONENT_CONFIG_PATH}")
     install(DIRECTORY ${LIBOCPP_CUSTOM_COMPONENT_CONFIG_PATH}/ DESTINATION ${CMAKE_INSTALL_DATADIR}/everest/modules/OCPP201/component_config/custom)
endif()
