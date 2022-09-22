if (NOT EVEREST_SCHEMA_DIR)
    get_filename_component(CUR_FILE_NAME ${CMAKE_CURRENT_LIST_FILE} NAME)
    message(FATAL_ERROR "\
The variable EVEREST_SCHEMA_DIR is not set, this needs to be done, \
before including \"${CUR_FILE_NAME}\"\
    ")
endif()

# NOTE (aw): maybe this could be also implemented as an IMPORTED target?
add_custom_target(generate_cpp_files)
set_target_properties(generate_cpp_files
    PROPERTIES
        EVEREST_SCHEMA_DIR "${EVEREST_SCHEMA_DIR}"
        EVEREST_GENERATED_OUTPUT_DIR "${CMAKE_BINARY_DIR}/generated"
        EVEREST_GENERATED_INCLUDE_DIR "${CMAKE_BINARY_DIR}/generated/include"
)

#
# interfaces
#

function (ev_add_interfaces)
    get_target_property(GENERATED_OUTPUT_DIR generate_cpp_files EVEREST_GENERATED_OUTPUT_DIR)
    set(CHECK_DONE_FILE "${GENERATED_OUTPUT_DIR}/.interfaces_generated_${PROJECT_NAME}")

    add_custom_command(
        OUTPUT
            "${CHECK_DONE_FILE}"
        DEPENDS
            ${ARGV}
        COMMENT
            "Generating/updating interface files ..."
        COMMAND
            ${EV_CLI} interface generate-headers
                --disable-clang-format
                --schemas-dir "$<TARGET_PROPERTY:generate_cpp_files,EVEREST_SCHEMA_DIR>"
                --output-dir "$<TARGET_PROPERTY:generate_cpp_files,EVEREST_GENERATED_INCLUDE_DIR>/generated/interfaces"
        COMMAND
            ${CMAKE_COMMAND} -E touch "${CHECK_DONE_FILE}"
        WORKING_DIRECTORY
            ${PROJECT_SOURCE_DIR}
    )

    add_custom_target(generate_interfaces_cpp_${PROJECT_NAME}
        DEPENDS "${CHECK_DONE_FILE}"
    )

    add_dependencies(generate_cpp_files
        generate_interfaces_cpp_${PROJECT_NAME}
    )
endfunction()

#
# types
#

function (ev_add_types)
    get_target_property(GENERATED_OUTPUT_DIR generate_cpp_files EVEREST_GENERATED_OUTPUT_DIR)
    set(CHECK_DONE_FILE "${GENERATED_OUTPUT_DIR}/.types_generated_${PROJECT_NAME}")

    add_custom_command(
        OUTPUT
            "${CHECK_DONE_FILE}"
        DEPENDS
            ${ARGV}
        COMMENT
            "Generating/updating type files ..."
        COMMAND
            ${EV_CLI} types generate-headers
                --disable-clang-format
                --schemas-dir "$<TARGET_PROPERTY:generate_cpp_files,EVEREST_SCHEMA_DIR>"
                --output-dir "$<TARGET_PROPERTY:generate_cpp_files,EVEREST_GENERATED_INCLUDE_DIR>/generated/types"
        COMMAND
            ${CMAKE_COMMAND} -E touch "${CHECK_DONE_FILE}"
        WORKING_DIRECTORY 
            ${PROJECT_SOURCE_DIR}
    )

    add_custom_target(generate_types_cpp_${PROJECT_NAME}
        DEPENDS
            ${CHECK_DONE_FILE}
    )

    add_dependencies(generate_cpp_files
        generate_types_cpp_${PROJECT_NAME}
    )
endfunction()

#
# modules
#

function (ev_add_modules)
    set(EVEREST_MODULE_INSTALL_PREFIX "${CMAKE_INSTALL_LIBEXECDIR}/everest/modules")

    # NOTE (aw): this logic could be customized in future
    foreach(MODULE ${ARGV})
        if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${ENTRY})
            list(APPEND MODULES ${MODULE})
            add_subdirectory(${MODULE})
        endif()
    endforeach()

    # FIXME (aw): this will override EVEREST_MODULES, might not what we want
    set_property(
        GLOBAL
        PROPERTY EVEREST_MODULES ${EVEREST_MODULES}
    )
endfunction()

function(ev_setup_cpp_module)
    get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    get_target_property(GENERATED_OUTPUT_DIR generate_cpp_files EVEREST_GENERATED_OUTPUT_DIR)

    set(GENERATED_MODULE_DIR "${GENERATED_OUTPUT_DIR}/modules")
    set(MODULE_LOADER_DIR ${GENERATED_MODULE_DIR}/${MODULE_NAME})

    add_custom_command(
        OUTPUT
            ${MODULE_LOADER_DIR}/ld-ev.hpp
            ${MODULE_LOADER_DIR}/ld-ev.cpp
        COMMAND
            ${EV_CLI} module generate-loader
                --disable-clang-format
                --schemas-dir "$<TARGET_PROPERTY:generate_cpp_files,EVEREST_SCHEMA_DIR>"
                --output-dir ${GENERATED_MODULE_DIR}
                ${MODULE_NAME}
        DEPENDS
            manifest.json
        WORKING_DIRECTORY
            ${PROJECT_SOURCE_DIR}
        COMMENT
            "Generating ld-ev for module ${MODULE_NAME}"
    )

    add_custom_target(ld-ev_${MODULE_NAME}
        DEPENDS ${MODULE_LOADER_DIR}/ld-ev.cpp
    )

    add_executable(${MODULE_NAME})

    target_include_directories(${MODULE_NAME}
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
            "$<TARGET_PROPERTY:generate_cpp_files,EVEREST_GENERATED_INCLUDE_DIR>"
            ${MODULE_LOADER_DIR}
    )

    target_sources(${MODULE_NAME}
        PRIVATE
            ${MODULE_NAME}.cpp
            "${MODULE_LOADER_DIR}/ld-ev.cpp"
    )

    target_link_libraries(${MODULE_NAME}
        PRIVATE
            everest::framework
    )

    add_dependencies(${MODULE_NAME} ld-ev_${MODULE_NAME} generate_cpp_files)

    install(TARGETS ${MODULE_NAME}
        DESTINATION "${EVEREST_MODULE_INSTALL_PREFIX}/${MODULE_NAME}"
    )

    install(FILES manifest.json
        DESTINATION "${EVEREST_MODULE_INSTALL_PREFIX}/${MODULE_NAME}"
    )

    set(MODULE_NAME ${MODULE_NAME} PARENT_SCOPE)
endfunction()

# FIXME (aw): this needs to be done
#set(FORCE_UPDATE_COMMANDS "")
#foreach(EVEREST_MODULE ${EVEREST_MODULES})
#    if(NOT ${EVEREST_MODULE} MATCHES "^Js")
#        list(APPEND FORCE_UPDATE_COMMANDS
#            COMMAND ${EV_CLI} module update --framework-dir ${EVEREST_DATADIR} -f ${EVEREST_MODULE}
#        )
#    endif()
#endforeach()
#
#add_custom_target(force_update_all_modules
#    ${FORCE_UPDATE_COMMANDS}
#    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
#)
