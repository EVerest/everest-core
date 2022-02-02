set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")

list(APPEND EVEREST_MODULES_LIST
    Charger
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
    SLAC_openplcutils
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

message("everest modules: ${EVEREST_MODULES}")
foreach(EVEREST_MODULE ${EVEREST_MODULES})
    add_subdirectory(${EVEREST_MODULE})
endforeach()

# FIXME (aw): because force_update_all_modules will be called directly
#             we might also choose to get all module names by listing
#             the current directory instead of using EVEREST_MODULES
#             (could be helpful while developing ...)
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
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..
)
