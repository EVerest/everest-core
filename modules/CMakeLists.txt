set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")

list(APPEND EVEREST_MODULES_LIST
    Charger
    EvseSlac
    Example
    ExampleUser
    JsAuth
    JsCarSimulator
    JsDummyTokenProvider
    JsDummyTokenValidator
    JsEmgrLogger
    JsEnergyManager
    JsForecastDotSolar
    JsPN532TokenProvider
    JsPowerIn
    JsRiseV2G
    JsSimPowerMeter
    JsTibber
    JsYetiSimulator
    ModbusMeter
    OCPP
    Power
    PowerIn
    Solar
    SunspecReader
    SunspecScanner
    Store
    YetiDriver
    EvseManager
)

set(EVEREST_MODULES "")
foreach(ENTRY ${EVEREST_MODULES_LIST})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${ENTRY})
        if(NOT "${ENTRY}" STREQUAL "CMakeFiles")
            list(APPEND EVEREST_MODULES ${ENTRY})
        endif()
    endif()
endforeach()

set(GENERATED_MODULE_DIR ${GENERATED_OUTPUT_DIR}/modules)


set(FORCE_UPDATE_COMMANDS "")
foreach(EVEREST_MODULE ${EVEREST_MODULES})
    if(NOT ${EVEREST_MODULE} MATCHES "^Js")
        list(APPEND FORCE_UPDATE_COMMANDS
            COMMAND ${EV_CLI} module update --framework-dir ${everest-framework_SOURCE_DIR} -f ${EVEREST_MODULE}
        )
    endif()
endforeach()

add_custom_target(force_update_all_modules
    ${FORCE_UPDATE_COMMANDS}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

macro(cpp_module_setup)
    get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    set(MOD_MODULE_NAME mod${MODULE_NAME})
    set(MODULE_LOADER_DIR ${GENERATED_MODULE_DIR}/${MODULE_NAME})
    add_library(${MOD_MODULE_NAME} SHARED)

    add_custom_command(
        OUTPUT ${MODULE_LOADER_DIR}/ld-ev.hpp ${MODULE_LOADER_DIR}/ld-ev.cpp
        COMMAND
            ${EV_CLI} module generate-loader --framework-dir ${everest-framework_SOURCE_DIR} --output-dir ${GENERATED_MODULE_DIR} ${MODULE_NAME}
        DEPENDS
            manifest.json
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT
            "Generating ld-ev for module ${MODULE_NAME}"
    )

    add_custom_target(ld-ev_${MODULE_NAME}
        DEPENDS ${MODULE_LOADER_DIR}/ld-ev.cpp
    )

    add_dependencies(${MOD_MODULE_NAME} ld-ev_${MODULE_NAME} generate_interface_files)
endmacro()

message("everest modules: ${EVEREST_MODULES}")
foreach(EVEREST_MODULE ${EVEREST_MODULES})
    add_subdirectory(${EVEREST_MODULE})
endforeach()
