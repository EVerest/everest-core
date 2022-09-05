file(GLOB TYPES_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.json
)

ev_add_types(${TYPES_FILES})
