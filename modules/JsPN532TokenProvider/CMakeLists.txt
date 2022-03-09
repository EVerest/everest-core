get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
add_custom_target(${MODULE_NAME} ALL)
add_dependencies(${MODULE_NAME} everestjs ${MODULE_NAME}_INSTALL_NODE_MODULES)

set(EVEREST_MOD_DESTINATION "modules/${MODULE_NAME}")

find_program(
    RSYNC
    rsync
    REQUIRED
)

find_program(
    NPM
    npm
    REQUIRED
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/package.json
    MAIN_DEPENDENCY package.json
    COMMENT "Copy package.json of module ${MODULE_NAME} to build dir"
    COMMAND ${RSYNC} -avq ${CMAKE_CURRENT_SOURCE_DIR}/package.json ${CMAKE_CURRENT_BINARY_DIR}/package.json
)

add_custom_command(
    OUTPUT .installed
    MAIN_DEPENDENCY ${CMAKE_CURRENT_BINARY_DIR}/package.json
    COMMENT "Installing dependencies of module ${MODULE_NAME} from package.json"
    COMMAND ${NPM} install > npm.log 2>&1 || ${CMAKE_COMMAND} -E cat ${CMAKE_CURRENT_BINARY_DIR}/npm.log
    COMMAND ${CMAKE_COMMAND} -E touch .installed
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(
    ${MODULE_NAME}_INSTALL_NODE_MODULES
    DEPENDS .installed
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
