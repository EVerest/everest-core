# FIXME (aw): quite hacky, should check at least if target already exists
add_library(mqttc STATIC IMPORTED)

find_library(MQTTC_LIB_FILE mqttc)
find_path(MQTTC_INCLUDE_DIR mqtt.h)

if(NOT MQTTC_LIB_FILE OR NOT MQTTC_INCLUDE_DIR)
    message(FATAL_ERROR "Could not find mqttc library")
endif()

set_target_properties(mqttc PROPERTIES
    IMPORTED_LOCATION ${MQTTC_LIB_FILE}
    INTERFACE_INCLUDE_DIRECTORIES ${MQTTC_INCLUDE_DIR}
)
