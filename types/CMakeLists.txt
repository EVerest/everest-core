file(GLOB TYPES_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.json
)

add_custom_target(generate_type_files
    DEPENDS
        .types_generated
)

# FIXME (aw): right now, the header files are generated in-source for convenience
add_custom_command(
    OUTPUT
        .types_generated
    DEPENDS
        ${TYPES_FILES}
    COMMENT
        "Generating/updating type files ..."
    COMMAND
        ${EV_CLI} types generate-headers --framework-dir ${everest-framework_SOURCE_DIR} --output-dir ${GENERATED_OUTPUT_DIR}/types
    COMMAND
        ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/.types_generated
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
