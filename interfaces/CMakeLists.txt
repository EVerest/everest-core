file(GLOB INTERFACE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.json
)

ev_add_interfaces(${INTERFACE_FILES})
