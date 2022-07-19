file(GLOB TYPES_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.json
)

add_custom_target(generate_type_files
    DEPENDS
        ${GENERATED_OUTPUT_DIR}/types
)

# FIXME (aw): right now, the header files are generated in-source for convenience
add_custom_command(
    OUTPUT
        ${GENERATED_OUTPUT_DIR}/types
    DEPENDS
        ${TYPES_FILES}
    COMMENT
        "Generating/updating type files ..."
    COMMAND
        ${EV_CLI} types generate-headers --framework-dir ${everest-framework_SOURCE_DIR} --output-dir ${GENERATED_OUTPUT_DIR}/types
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
