if (OPT_AUTODOWNLOAD_ISO20_SCHEMAS)

    find_program(WGET_EXECUTABLE wget)

    if (NOT WGET_EXECUTABLE)
        message(FATAL_ERROR "Cannot find wget executable to download schema files.  Please install wget.")
    endif ()

    list(APPEND ISO20_SCHEMA_FILES
        V2G_CI_AC.xsd
        V2G_CI_ACDP.xsd
        V2G_CI_AppProtocol.xsd
        V2G_CI_CommonMessages.xsd
        V2G_CI_CommonTypes.xsd
        V2G_CI_DC.xsd
        V2G_CI_WPT.xsd
        xmldsig-core-schema.xsd
    )

    set(ISO20_BASE_URL "https://standards.iso.org/iso/15118/-20/ed-1/en/")

    set(ISO20_SCHEMA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/schema/iso-20)

    if (NOT EXISTS ${ISO20_SCHEMA_DIR})
        file(MAKE_DIRECTORY ${ISO20_SCHEMA_DIR})
    endif ()

    foreach (ISO20_SCHEMA_FILE ${ISO20_SCHEMA_FILES})
        if (EXISTS "${ISO20_SCHEMA_DIR}/${ISO20_SCHEMA_FILE}")
            continue()
        endif ()

        message(STATUS "Downloading ${ISO20_SCHEMA_FILE}")
        execute_process(
            COMMAND
                ${WGET_EXECUTABLE} -P ${ISO20_SCHEMA_DIR} ${ISO20_BASE_URL}${ISO20_SCHEMA_FILE}
            RESULT_VARIABLE
                DOWNLOAD_FAILED
            ERROR_VARIABLE
                DOWNLOAD_ERROR_MSG
        )

        if (DOWNLOAD_FAILED)
            message(FATAL_ERROR "Download of ${ISO20_BASE_URL}${ISO20_SCHEMA_FILE} failed.  Reason:\n${DOWNLOAD_ERROR_MSG}")
        endif ()
    endforeach()

endif() # OPT_AUTODOWNLOAD_ISO20_SCHEMAS
