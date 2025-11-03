set(MODULE_LINK_PATH "${CMAKE_INSTALL_PREFIX}/${EVEREST_MOD_DESTINATION}")

if(IS_DIRECTORY ${MODULE_LINK_PATH} AND NOT IS_SYMLINK ${MODULE_LINK_PATH})
    message(
        FATAL_ERROR "\
I won't be able create the symlink, because the link target \
${MODULE_LINK_PATH} already exists and I don't want to delete \
it. Probably you executed the INSTALL target already without \
symlinks. Please remove the directory manually.\
"
    )
endif()

install(
    CODE "
        execute_process(COMMAND cmake -E create_symlink
                        ${CMAKE_CURRENT_SOURCE_DIR}
                        ${MODULE_LINK_PATH})
    "
)