# Copy all schema files (yaml) to the install directory
# and add a compile definition to it so the path can be used in code.

set("${PROJECT_NAME}_${MODULE_NAME}_MAPPING_SCHEMA_DIR" "${CMAKE_INSTALL_PREFIX}/etc/everest/ocpp-configuration/mapping_schemas")

install(DIRECTORY "."
        DESTINATION "${${PROJECT_NAME}_${MODULE_NAME}_MAPPING_SCHEMA_DIR}"
        FILES_MATCHING PATTERN "*.yaml")

target_compile_definitions(${MODULE_NAME} PRIVATE MAPPING_SCHEMA_INSTALL_DIRECTORY="${${PROJECT_NAME}_${MODULE_NAME}_MAPPING_SCHEMA_DIR}")
