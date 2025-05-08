function (setup_test_directory)
    # NOTE (aw): this function got quite complex, it could be simpler if
    # the tests would use a more consistent directory layout

    set(options
        USE_FILESYSTEM_HIERARCHY_STANDARD
    )

    set(one_value_args
        CONFIG
        USER_CONFIG
    )
    set(multi_value_args
        TYPE_FILES
        MODULES
        INTERFACE_FILES
    )

    cmake_parse_arguments(arg "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    if (NOT arg_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} requires first argument to be the name")
    endif()

    list(GET arg_UNPARSED_ARGUMENTS 0 NAME)
    if (NOT NAME STREQUAL ARGV0)
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} unexpected argument: ${NAME}")
    endif()

    list(LENGTH arg_UNPARSED_ARGUMENTS UNPARSED_ARGUMENTS_COUNT)
    if (UNPARSED_ARGUMENTS_COUNT GREATER 1)
        list(GET arg_UNPARSED_ARGUMENTS 1 MODULE)
        if (NOT MODULE STREQUAL ARGV1)
            message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} unexpected argument: ${MODULE}")
        endif()
    endif()

    if (UNPARSED_ARGUMENTS_COUNT GREATER 2)
        list(GET arg_UNPARSED_ARGUMENTS 2 INTERFACE_FILE)
        if (NOT INTERFACE_FILE STREQUAL ARGV2)
            message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} unexpected argument: ${INTERFACE_FILE}")
        endif()
    endif()

    if (UNPARSED_ARGUMENTS_COUNT GREATER 3)
        list(GET arg_UNPARSED_ARGUMENTS 3 UNKNOWN_ARG)
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} unknown argument: ${UNKNOWN_ARG}")
    endif()

    set(DIR ${CMAKE_CURRENT_BINARY_DIR}/${NAME})
    file(MAKE_DIRECTORY "${DIR}")

    set(SHARE_EVEREST_DIR ${DIR})
    if (arg_USE_FILESYSTEM_HIERARCHY_STANDARD)
        set(SHARE_EVEREST_DIR "${DIR}/share/everest")
    endif ()
    file(MAKE_DIRECTORY "${SHARE_EVEREST_DIR}")

    set (SCHEMAS_DIR "${SHARE_EVEREST_DIR}/schemas")
    file(MAKE_DIRECTORY "${SCHEMAS_DIR}")
    file(COPY ${PROJECT_SOURCE_DIR}/schemas/ DESTINATION ${SCHEMAS_DIR}/)

    set (INTERFACES_DIR "${SHARE_EVEREST_DIR}/interfaces")
    file(MAKE_DIRECTORY "${INTERFACES_DIR}")

    set (TYPES_DIR "${SHARE_EVEREST_DIR}/types")
    file(MAKE_DIRECTORY "${TYPES_DIR}")


    file(MAKE_DIRECTORY "${SHARE_EVEREST_DIR}/errors")
    file(MAKE_DIRECTORY "${SHARE_EVEREST_DIR}/www")


    set(MODULES_DIR "${DIR}/modules")
    if (arg_USE_FILESYSTEM_HIERARCHY_STANDARD)
        set(MODULES_DIR "${DIR}/libexec/everest/modules")
    endif()
    file(MAKE_DIRECTORY "${MODULES_DIR}")

    set(CONFIG_DIR ${DIR})
    if (arg_USE_FILESYSTEM_HIERARCHY_STANDARD)
        set(CONFIG_DIR "${DIR}/etc/everest")
    endif ()
    file(MAKE_DIRECTORY "${CONFIG_DIR}")

    if (arg_USE_FILESYSTEM_HIERARCHY_STANDARD)
        configure_file(test_logging.ini ${CONFIG_DIR}/default_logging.cfg COPYONLY)
    else ()
        configure_file(test_logging.ini ${CONFIG_DIR}/logging.ini COPYONLY)
    endif()

    # FIXME (aw): these two need to exist anyway
    file(MAKE_DIRECTORY "${DIR}/etc/everest")
    file(MAKE_DIRECTORY "${DIR}/share/everest")


    # FIXME (aw): config files get special directory treatment, this
    # should be consistent too (tests need to be changed)
    if (arg_CONFIG)
        # NOTE (aw): this is for json config file compatibility, but
        # this is probably not in use anymore and should be removed
        get_filename_component(CONFIG_EXT ${arg_CONFIG} LAST_EXT)
        configure_file(test_configs/${arg_CONFIG} ${DIR}/config${CONFIG_EXT} COPYONLY)
    else ()
        configure_file(test_configs/${NAME}_config.yaml ${DIR}/config.yaml COPYONLY)
    endif()

    if (arg_USER_CONFIG)
        configure_file(test_configs/${arg_USER_CONFIG} ${DIR}/user-config/config.yaml COPYONLY)
    endif()

    if (MODULE)
        file(COPY test_modules/${MODULE} DESTINATION ${MODULES_DIR})
    endif()

    if (INTERFACE_FILE)
        file(COPY test_interfaces/${INTERFACE_FILE}.yaml DESTINATION ${INTERFACES_DIR}/)
    endif()

    if (arg_TYPE_FILES)
        foreach(TYPE_FILE ${arg_TYPE_FILES})
            file(COPY test_types/${TYPE_FILE} DESTINATION ${TYPES_DIR}/)
        endforeach()
    endif()
    if (arg_MODULES)
        foreach(MODULE ${arg_MODULES})
            file(COPY test_modules/${MODULE} DESTINATION ${MODULES_DIR})
        endforeach()
    endif()
    if (arg_INTERFACE_FILES)
        foreach(INTERFACE_FILE ${arg_INTERFACE_FILES})
            file(COPY test_interfaces/${INTERFACE_FILE}.yaml DESTINATION ${INTERFACES_DIR}/)
        endforeach()
    endif()

endfunction()
