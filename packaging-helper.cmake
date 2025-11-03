# default paths for install directories
include(GNUInstallDirs)

include(CMakePackageConfigHelpers)

function(evc_get_main_project_flag RETVAL)
    get_directory_property(_HAS_PARENT PARENT_DIRECTORY)
    if(_HAS_PARENT)
        set(${RETVAL} FALSE PARENT_SCOPE)
    else()
        set(${RETVAL} TRUE PARENT_SCOPE)
    endif()
endfunction()

function(_parse_path_vars PATH_VARS PATH_VARS_ARG INLINE_CONTENT ERROR)
    list(LENGTH ${PATH_VARS} PATH_VARS_LENGTH)
    math(EXPR PATH_VARS_LENGTH_MODULO_TWO "${PATH_VARS_LENGTH} % 2")
    if (PATH_VARS_LENGTH_MODULO_TWO)
        set(${ERROR} "needs an even number of arguments" PARENT_SCOPE)
        return()
    endif()

    math(EXPR PAIR_COUNT "${PATH_VARS_LENGTH} / 2")

    set(_PATH_VARS_ARG "PATH_VARS")

    foreach(PAIR_IDX RANGE 1 ${PAIR_COUNT})
        math(EXPR VAR_IDX "(${PAIR_IDX}-1)*2")
        math(EXPR VALUE_IDX "${PAIR_IDX}*2-1")
        list(GET ${PATH_VARS} ${VAR_IDX} CUR_PAIR_VAR)
        list(GET ${PATH_VARS} ${VALUE_IDX} CUR_PAIR_VALUE)

        # create holding variable in parent scope
        set(${CUR_PAIR_VAR} ${CUR_PAIR_VALUE} PARENT_SCOPE)

        list(APPEND _PATH_VARS_ARG "${CUR_PAIR_VAR}")
    endforeach()

    # write through return values
    set(${PATH_VARS_ARG} ${_PATH_VARS_ARG} PARENT_SCOPE)
endfunction()

function(evc_setup_package)
    #
    # handle passed arguments
    #
    set(options "")
    set(one_value_args
        EXPORT
        NAME
        NAMESPACE
        VERSION
        COMPATIBILITY
    )
    set(multi_value_args
        ADDITIONAL_CONTENT
        PATH_VARS
    )

    cmake_parse_arguments(OPTNS "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    if (OPTNS_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} got unknown argument(s): ${OPTNS_UNPARSED_ARGUMENTS}")
    endif()

    if (NOT OPTNS_NAME)
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} requires NAME parameter for the package name")
    endif()

    set(LIBRARY_PACKAGE_NAME "${OPTNS_NAME}")
    set(LIBRARY_PACKAGE_CMAKE_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_PACKAGE_NAME})

    if (NOT OPTNS_EXPORT)
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} requires EXPORT paramater for the package export set")
    endif()

    set(LIBRARY_PACKAGE_EXPORT_TARGETS ${OPTNS_EXPORT})

    set(LIBRARY_PACKAGE_VERSION ${PROJECT_VERSION})
    if (OPTNS_VERSION)
        set(LIBRARY_PACKAGE_VERSION ${OPTNS_VERSION})
    endif()

    set(LIBRARY_PACKAGE_VERSION_COMPATIBILITY ExactVersion)
    if (OPTNS_COMPATIBILITY)
        set(LIBRARY_PACKAGE_VERSION_COMPATIBILITY ${OPTNS_COMPATIBILITY})
    endif()

    if (OPTNS_PATH_VARS)
        _parse_path_vars(OPTNS_PATH_VARS PATH_VARS_ARG INLINE_CONTENT PARSE_ERROR)
        if (PARSE_ERROR)
            message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION}: error parsing PATH_VARS (${PARSE_ERROR})")
        endif()

    endif()

    if (OPTNS_ADDITIONAL_CONTENT)
        set(CONFIG_ADDITIONAL_CONTENT "")
        foreach(ITEM ${OPTNS_ADDITIONAL_CONTENT})
            string(APPEND CONFIG_ADDITIONAL_CONTENT "${ITEM}\n")
        endforeach()
    endif()

    #
    # install file containing the targets of this package
    #
    if (OPTNS_NAMESPACE)
        set(NAMESPACE_ARG "NAMESPACE;${OPTNS_NAMESPACE}::")
    endif()

    install(
        EXPORT ${LIBRARY_PACKAGE_EXPORT_TARGETS}
        FILE "${LIBRARY_PACKAGE_NAME}-targets.cmake"
        ${NAMESPACE_ARG}
        DESTINATION ${LIBRARY_PACKAGE_CMAKE_INSTALL_DIR}
    )

    #
    # generate package config file and install it
    #
    set(USER_PACKAGE_CONFIG_IN_FILE ${PROJECT_SOURCE_DIR}/config.cmake.in)
    set(PACKAGE_CONFIG_IN_FILE ${CMAKE_CURRENT_BINARY_DIR}/config.cmake.in)

    # check for user defined package config file, otherwise use default one
    if (EXISTS ${USER_PACKAGE_CONFIG_IN_FILE})
        set(PACKAGE_CONFIG_IN_FILE ${USER_PACKAGE_CONFIG_IN_FILE})
    else()
        string(CONCAT DEFAULT_PACKAGE_CONFIG_CONTENT
            "@PACKAGE_INIT@\n\n"
            "include(\${CMAKE_CURRENT_LIST_DIR}/${LIBRARY_PACKAGE_NAME}-targets.cmake)\n\n"
            "include(CMakeFindDependencyMacro)\n"
            "${INLINE_CONTENT}"
            "${CONFIG_ADDITIONAL_CONTENT}\n"
            "check_required_components(${LIBRARY_PACKAGE_NAME})\n"
        )
        file(WRITE ${PACKAGE_CONFIG_IN_FILE} ${DEFAULT_PACKAGE_CONFIG_CONTENT})
    endif()

    configure_package_config_file(
        ${PACKAGE_CONFIG_IN_FILE}
        ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_PACKAGE_NAME}-config.cmake
        INSTALL_DESTINATION ${LIBRARY_PACKAGE_CMAKE_INSTALL_DIR}
        ${PATH_VARS_ARG}
    )

    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_PACKAGE_NAME}-config-version.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY ExactVersion
    )

    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_PACKAGE_NAME}-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_PACKAGE_NAME}-config-version.cmake
        DESTINATION ${LIBRARY_PACKAGE_CMAKE_INSTALL_DIR}
    )
endfunction()
