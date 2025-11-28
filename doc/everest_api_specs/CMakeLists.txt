if(EVEREST_BUILD_API_DOCS)
    include(${PROJECT_SOURCE_DIR}/cmake/fetch_async_api.cmake)
    include(${PROJECT_SOURCE_DIR}/cmake/fetch_async_api_html_template.cmake)
endif()

# Create for each API a list with: folder name, api camel case, api snake case and the prefix
set(SET01 "auth_consumer_API" "AuthConsumerAPI" "auth_consumer_api" "tc")
set(SET02 "auth_token_provider_API" "AuthTokenProviderAPI" "auth_token_provider_api" "tp")
set(SET03 "auth_token_validator_API" "AuthTokenValidatorAPI" "auth_token_validator_api" "tv")
set(SET04 "dc_external_derate_consumer_API" "DcExternalDerateConsumerAPI" "dc_external_derate_consumer_api" "ded")
set(SET05 "display_message_API" "DisplayMessageAPI" "display_message_api" "dm")
set(SET06 "error_history_consumer_API" "ErrorHistoryConsumerAPI" "error_history_consumer_api" "eh")
set(SET07 "evse_board_support_API" "EvseBspAPI" "evse_bsp_api" "bsp")
set(SET08 "evse_manager_consumer_API" "EvseManagerConsumerAPI" "evse_manager_consumer_api" "em")
set(SET09 "external_energy_limits_consumer_API" "ExternalEnergyLimitsConsumerAPI" "external_energy_limits_consumer_api" "eel")
set(SET10 "generic_error_raiser_API" "GenericErrorRaiserAPI" "generic_error_raiser_api" "err")
set(SET11 "isolation_monitor_API" "IsolationMonitorAPI" "isolation_monitor_api" "im")
set(SET12 "ocpp_consumer_API" "OcppConsumerAPI" "ocpp_consumer_api" "oc")
set(SET13 "over_voltage_monitor_API" "OverVoltageMonitorAPI" "over_voltage_monitor_api" "ovm")
set(SET14 "power_supply_DC_API" "PowerSupplyDCAPI" "power_supply_dc_api" "ps")
set(SET15 "powermeter_API" "PowermeterAPI" "powermeter_api" "pm")
set(SET16 "session_cost_API" "SessionCostAPI" "session_cost_api" "sc")
set(SET17 "session_cost_consumer_API" "SessionCostConsumerAPI" "session_cost_api" "scc")
set(SET18 "system_API" "SystemAPI" "system_api" "sys")



# create a list to iterate over (for documetation generation and client)
set(API_SPECS "SET01" "SET02" "SET03" "SET04" "SET05" "SET06" "SET07" "SET08" "SET09" "SET10" "SET11" "SET12" "SET13" "SET14" "SET15" "SET16" "SET17" "SET18")

# Iterate over the list and create the documentation
if(EVEREST_BUILD_API_DOCS)
    include(${PROJECT_SOURCE_DIR}/cmake/generate-api-docs.cmake)

    add_custom_target(everest_api_docs ALL)

    foreach(API_SPEC IN LISTS API_SPECS)
        set(API_SPEC_PARAMS ${${API_SPEC}})
        list(GET API_SPEC_PARAMS 0 API_NAME)
        message(STATUS "Documentation generation for: ${API_NAME}")
        generate_async_api_docs(
            API_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${API_NAME}/asyncapi.yaml
            API_NAME ${API_NAME}
        )
    endforeach()
endif()

