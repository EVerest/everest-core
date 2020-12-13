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
    Power
    PowerIn
    SLAC_openplcutils
    Solar
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
