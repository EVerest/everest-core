# search for ev-cli tool
find_program(EV_CLI
    ev-cli
    REQUIRED
)

set(INTERFACE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../interfaces)

file(GLOB INTERFACE_FILES
    ${INTERFACE_DIR}/*.json
)

add_custom_target(generate_interface_files
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

# FIXME (aw): right now, the header files are generated in-source for convenience
add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DEPENDS
        ${INTERFACE_FILES}
    COMMENT
        "Generating/updating interface files ..."
    COMMAND
        ${EV_CLI} interface generate-headers --framework-dir ${everest-framework_SOURCE_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# add target for generated headers
set(GENERATED_HEADER_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/../generated/include"
    PARENT_SCOPE
)
