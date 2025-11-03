#
# AUTO GENERATED - MARKED REGIONS WILL BE KEPT
# template version 0.0.1
#

set(MODULE_NAME "YetiDriver")
set(MOD_MODULE_NAME "mod${MODULE_NAME}")
add_library(${MOD_MODULE_NAME} SHARED)
add_dependencies(${MOD_MODULE_NAME} generate_interface_files)

# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1
add_subdirectory(yeti_comms)
add_subdirectory(yeti_fwupdate)

list(APPEND ADDITIONAL_MODULE_INCLUDES
    "common"
    "yeti_comms"
    "yeti_comms/nanopb"
    "yeti_comms/protobuf"
)
list(APPEND ADDITIONAL_MODULE_LIBS
    Pal::Sigslot
    yeti_comms
)
# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1

target_include_directories(${MOD_MODULE_NAME}
    PUBLIC
        "everest"
        ${GENERATED_HEADER_DIR}
        ${ADDITIONAL_MODULE_INCLUDES}
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(${MOD_MODULE_NAME}
    PRIVATE
        ${MODULE_NAME}.cpp
        "ld-ev.cpp"
        "powermeter/powermeterImpl.cpp"
        "board_support/board_support_ACImpl.cpp"
        "yeti_extras/yeti_extrasImpl.cpp"
        "debug_yeti/debug_jsonImpl.cpp"
        "debug_powermeter/debug_jsonImpl.cpp"
        "debug_state/debug_jsonImpl.cpp"
        "debug_keepalive/debug_jsonImpl.cpp"
        "yeti_simulation_control/yeti_simulation_controlImpl.cpp"
        ${ADDITIONAL_MODULE_SOURCES}
)

target_link_libraries(${MOD_MODULE_NAME}
    PRIVATE
        everest ${CMAKE_DL_LIBS}
        everest::log
        nlohmann_json::nlohmann_json
        ${ADDITIONAL_MODULE_LIBS}
)

target_compile_features(${MOD_MODULE_NAME} PRIVATE cxx_std_14)

set(YETIDRIVER_DEST "modules/${MODULE_NAME}")
install(TARGETS ${MOD_MODULE_NAME}
    LIBRARY
    DESTINATION ${YETIDRIVER_DEST}
)

install(FILES manifest.json
    DESTINATION ${YETIDRIVER_DEST}
)

# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
# insert other things like install cmds etc here
# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
