get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
add_custom_target(${MODULE_NAME} ALL)
add_dependencies(${MODULE_NAME} everestjs)
set(EVEREST_MOD_DESTINATION "modules/${MODULE_NAME}")

find_program(
    RSYNC
    rsync
    REQUIRED
)
add_custom_target(
    ${MODULE_NAME}_package_json ALL
    COMMENT "Copy package.json of module ${MODULE_NAME} to build dir"
    COMMAND ${RSYNC} -av ${CMAKE_CURRENT_SOURCE_DIR}/package.json ${CMAKE_CURRENT_BINARY_DIR}/package.json
)

find_program(
    NPM
    npm
    REQUIRED
)
add_custom_target(
    ${MODULE_NAME}_npm_dependencies ALL
    COMMENT "Installing dependencies of module ${MODULE_NAME} from package.json"
    COMMAND ${NPM} install
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

install(
    DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/node_modules
    DESTINATION ${EVEREST_MOD_DESTINATION}
)

# install the whole js project
if(CREATE_SYMLINKS)
    include("CreateModuleSymlink")
else()
    install(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
        DESTINATION ${EVEREST_MOD_DESTINATION}
        PATTERN "CMakeLists.txt" EXCLUDE
        PATTERN "CMakeFiles" EXCLUDE)
endif()
