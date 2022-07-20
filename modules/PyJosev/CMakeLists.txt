get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
add_custom_target(${MODULE_NAME} ALL)
# add_dependencies(${MODULE_NAME} everestpy)
set(EVEREST_MOD_DESTINATION "modules/${MODULE_NAME}")

# install the whole python project
install(
    FILES 
        module.py
        everest_iso15118.py
        manifest.json
    DESTINATION "${EVEREST_MODULE_INSTALL_PREFIX}/${MODULE_NAME}"
)