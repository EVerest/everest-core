get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
add_custom_target(${MODULE_NAME} ALL)
# add_dependencies(${MODULE_NAME} everestpy)

# install the whole python project
install(FILES module.py manifest.yaml
    DESTINATION "${EVEREST_MODULE_INSTALL_PREFIX}/${MODULE_NAME}"
)
