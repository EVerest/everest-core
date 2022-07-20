get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
add_custom_target(${MODULE_NAME} ALL)

# install the whole js project
if(CREATE_SYMLINKS)
    include("CreateModuleSymlink")
else()
    install(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
        DESTINATION "${EVEREST_MODULE_INSTALL_PREFIX}/${MODULE_NAME}"
        PATTERN "CMakeLists.txt" EXCLUDE
        PATTERN "CMakeFiles" EXCLUDE)
endif()
