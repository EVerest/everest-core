file(GLOB INTERFACE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.json
)

add_custom_target(generate_interface_files
    DEPENDS
        ${GENERATED_HEADER_DIR}
)

# FIXME (aw): right now, the header files are generated in-source for convenience
add_custom_command(
    OUTPUT
        ${GENERATED_HEADER_DIR}
    DEPENDS
        ${INTERFACE_FILES}
    COMMENT
        "Generating/updating interface files ..."
    COMMAND
        ${EV_CLI} interface generate-headers --framework-dir ${everest-framework_SOURCE_DIR} --output-dir ${GENERATED_HEADER_DIR}/generated
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
