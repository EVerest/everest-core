list(APPEND AUX_DIRS interfaces)

foreach(IT IN LISTS AUX_DIRS)
    set(IT_DST "${CMAKE_INSTALL_PREFIX}/${IT}")

    if(IS_DIRECTORY ${IT_DST} AND NOT IS_SYMLINK ${IT_DST})
        message(
            FATAL_ERROR "\
I won't be able create the symlink, because the link target ${IT_DST} \
already exists and I don't want to delete it.  Probably you executed \
the INSTALL target already without symlinks.  Please remove the \
directory manually.\
"
        )
    endif()

    install(
        CODE "
            execute_process(COMMAND cmake -E create_symlink
                            ${CMAKE_CURRENT_SOURCE_DIR}/${IT}
                            ${IT_DST})
        "
    )
endforeach()
