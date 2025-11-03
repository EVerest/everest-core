get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
add_custom_target(${MODULE_NAME} ALL)
add_dependencies(${MODULE_NAME} everestjs)
set(EVEREST_MOD_DESTINATION "modules/${MODULE_NAME}")

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
