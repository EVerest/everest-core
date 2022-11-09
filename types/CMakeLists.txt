file(GLOB TYPES_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.yaml
)

ev_add_types(${TYPES_FILES})
