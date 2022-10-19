file(GLOB INTERFACE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.yaml
)

ev_add_interfaces(${INTERFACE_FILES})
