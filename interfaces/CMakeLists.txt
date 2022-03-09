file(GLOB INTERFACE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.json
)

add_custom_target(generate_interface_files
    DEPENDS
        .interfaces_generated
)

# FIXME (aw): right now, the header files are generated in-source for convenience
add_custom_command(
    OUTPUT
        .interfaces_generated
    DEPENDS
        ${INTERFACE_FILES}
    COMMENT
        "Generating/updating interface files ..."
    COMMAND
        ${EV_CLI} interface generate-headers --framework-dir ${everest-framework_SOURCE_DIR} --output-dir ${GENERATED_HEADER_DIR}/generated
    COMMAND
        ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/.interfaces_generated
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
