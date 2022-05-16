#
# AUTO GENERATED - MARKED REGIONS WILL BE KEPT
# template version 2
#

# module setup:
#   - ${MODULE_NAME}: module name
cpp_module_setup()

# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1
list(APPEND ADDITIONAL_MODULE_LIBS
    slac::slac
    fsm::fsm
    fmt::fmt
)
list(APPEND ADDITIONAL_MODULE_SOURCES
    main/slac_io.cpp
    main/evse_fsm.cpp
)
# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1

target_include_directories(${MODULE_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${GENERATED_HEADER_DIR}
        ${MODULE_LOADER_DIR}
        ${ADDITIONAL_MODULE_INCLUDES}
)

target_sources(${MODULE_NAME}
    PRIVATE
        ${MODULE_NAME}.cpp
        "${MODULE_LOADER_DIR}/ld-ev.cpp"
        "main/slacImpl.cpp"
        ${ADDITIONAL_MODULE_SOURCES}
)

target_link_libraries(${MODULE_NAME}
    PRIVATE
        everest ${CMAKE_DL_LIBS}
        everest::log
        Boost::thread
        Boost::program_options
        fmt::fmt
        mqttc
        nlohmann_json::nlohmann_json
        ${ADDITIONAL_MODULE_LIBS}
)

target_compile_features(${MODULE_NAME} PRIVATE cxx_std_14)

set(EVSESLAC_DEST "modules/${MODULE_NAME}")

install(TARGETS ${MODULE_NAME}
    DESTINATION ${EVSESLAC_DEST}
)

install(FILES manifest.json
    DESTINATION ${EVSESLAC_DEST}
)

target_link_libraries(${MODULE_NAME}
    PRIVATE
        date::date
        date::date-tz
)
# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
# insert other things like install cmds etc here
# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
