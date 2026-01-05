function(collect_migration_files)
    set(options "")
    set(oneValueArgs LOCATION INSTALL_DESTINATION)
    set(multiValueArgs "")
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT ARG_LOCATION)
        message(FATAL_ERROR "No LOCATION provided, can't parse files")
    endif()

    if(NOT ${ARG_LOCATION} MATCHES "/$")
        set(ARG_LOCATION "${ARG_LOCATION}/")
    endif()

    message("Parsing migration files in folder: ${ARG_LOCATION}")

    file(GLOB MIGRATION_FILE_LIST RELATIVE ${ARG_LOCATION} "${ARG_LOCATION}*.sql") # ARG_LOCATION already contains the slash
    list(SORT MIGRATION_FILE_LIST)

    # The first file should always start with 1_up so make use of that fact.
    # Next we always check the next number "down" and then "up" which come in order since we sorted alphabetically
    set(CURRENT_MIGRATION_FILE_ID 1)
    set(NEXT_MIGRATION_FILE_TYPE "up")

    foreach(MIGRATION_FILE ${MIGRATION_FILE_LIST})
        string(REGEX MATCH "^([0-9]+)_(up|down)(|-.+)\.sql$" MIGRATION_FILE_MATCHED ${MIGRATION_FILE})
        if (MIGRATION_FILE_MATCHED STREQUAL "")
            message(FATAL_ERROR "Migration filename does not match specification: " ${MIGRATION_FILE})
        endif()

        string(CONCAT NEXT_ID "^" ${CURRENT_MIGRATION_FILE_ID} "_")
        if(NOT ${MIGRATION_FILE_MATCHED} MATCHES ${NEXT_ID})
            message(FATAL_ERROR "Skipped migration file ID, expected " ${CURRENT_MIGRATION_FILE_ID} "_*.sql, but got " ${MIGRATION_FILE_MATCHED})
        endif()

        string(APPEND NEXT_ID ${NEXT_MIGRATION_FILE_TYPE})
        if(NOT ${MIGRATION_FILE_MATCHED} MATCHES ${NEXT_ID})
            message(FATAL_ERROR "Missing " ${NEXT_MIGRATION_FILE_TYPE} " migration file: " ${MIGRATION_FILE_MATCHED})
        endif()

        if(NEXT_MIGRATION_FILE_TYPE STREQUAL "up")
            math(EXPR CURRENT_MIGRATION_FILE_ID "${CURRENT_MIGRATION_FILE_ID}+1")
            set(NEXT_MIGRATION_FILE_TYPE "down")
        elseif(NEXT_MIGRATION_FILE_TYPE STREQUAL "down")
            set(NEXT_MIGRATION_FILE_TYPE "up")
        endif()
    endforeach()

    if (NEXT_MIGRATION_FILE_TYPE STREQUAL "up")
        message(FATAL_ERROR "Down migration file " ${CURRENT_MIGRATION_FILE_ID} "_*.sql is missing up migration file")
    endif()

    # Since we always add on the up file we need to subtract one here
    math(EXPR CURRENT_MIGRATION_FILE_ID "${CURRENT_MIGRATION_FILE_ID}-1")

    list(TRANSFORM MIGRATION_FILE_LIST PREPEND ${ARG_LOCATION})
    if(ARG_INSTALL_DESTINATION)
        install(FILES ${MIGRATION_FILE_LIST} DESTINATION ${ARG_INSTALL_DESTINATION})
    endif()

    set(TARGET_MIGRATION_FILE_VERSION ${CURRENT_MIGRATION_FILE_ID} PARENT_SCOPE)
    set(MIGRATION_FILE_LIST ${MIGRATION_FILE_LIST} PARENT_SCOPE)
endfunction()