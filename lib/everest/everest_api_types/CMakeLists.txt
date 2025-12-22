# SO for API with structs and (de)serialization codec
add_library(everest_api_types SHARED)
add_library(everest::everest_api_types ALIAS everest_api_types)
ev_register_library_target(everest_api_types)

add_dependencies(everest_api_types generate_cpp_files)

target_compile_options(everest_api_types
  PUBLIC  -Wall -Wextra -pedantic -Werror=switch)

set_target_properties(everest_api_types PROPERTIES
  VERSION 0.1.0
  SOVERSION 0
)

target_include_directories(everest_api_types
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/private_include>

  PUBLIC $<INSTALL_INTERFACE:include>

  PRIVATE   "private_include/everest_api_types"
  PRIVATE   "include/everest_api_types"
  PRIVATE   "${CMAKE_BINARY_DIR}/generated/include"
)

target_sources(everest_api_types
  PRIVATE

  src/everest_api_types/auth/codec.cpp
  src/everest_api_types/auth/json_codec.cpp
  src/everest_api_types/auth/wrapper.cpp

  src/everest_api_types/dc_external_derate/codec.cpp
  src/everest_api_types/dc_external_derate/json_codec.cpp
  src/everest_api_types/dc_external_derate/wrapper.cpp

  src/everest_api_types/display_message/codec.cpp
  src/everest_api_types/display_message/json_codec.cpp
  src/everest_api_types/display_message/wrapper.cpp

  src/everest_api_types/energy/codec.cpp
  src/everest_api_types/energy/json_codec.cpp
  src/everest_api_types/energy/wrapper.cpp

  src/everest_api_types/error_history/codec.cpp
  src/everest_api_types/error_history/json_codec.cpp
  src/everest_api_types/error_history/wrapper.cpp

  src/everest_api_types/evse_board_support/codec.cpp
  src/everest_api_types/evse_board_support/json_codec.cpp
  src/everest_api_types/evse_board_support/wrapper.cpp

  src/everest_api_types/evse_manager/codec.cpp
  src/everest_api_types/evse_manager/json_codec.cpp
  src/everest_api_types/evse_manager/wrapper.cpp

  src/everest_api_types/money/codec.cpp
  src/everest_api_types/money/json_codec.cpp
  src/everest_api_types/money/wrapper.cpp

  src/everest_api_types/iso15118_charger/codec.cpp
  src/everest_api_types/iso15118_charger/json_codec.cpp
  src/everest_api_types/iso15118_charger/wrapper.cpp

  src/everest_api_types/isolation_monitor/codec.cpp
  src/everest_api_types/isolation_monitor/json_codec.cpp
  src/everest_api_types/isolation_monitor/wrapper.cpp

  src/everest_api_types/ocpp/codec.cpp
  src/everest_api_types/ocpp/json_codec.cpp
  src/everest_api_types/ocpp/wrapper.cpp

  src/everest_api_types/over_voltage_monitor/codec.cpp
  src/everest_api_types/over_voltage_monitor/json_codec.cpp

  src/everest_api_types/power_supply_DC/codec.cpp
  src/everest_api_types/power_supply_DC/json_codec.cpp
  src/everest_api_types/power_supply_DC/wrapper.cpp

  src/everest_api_types/powermeter/codec.cpp
  src/everest_api_types/powermeter/json_codec.cpp
  src/everest_api_types/powermeter/wrapper.cpp

  src/everest_api_types/json_rpc_api/codec.cpp
  src/everest_api_types/json_rpc_api/json_codec.cpp

  src/everest_api_types/session_cost/codec.cpp
  src/everest_api_types/session_cost/json_codec.cpp
  src/everest_api_types/session_cost/wrapper.cpp

  src/everest_api_types/slac/codec.cpp
  src/everest_api_types/slac/json_codec.cpp

  src/everest_api_types/system/codec.cpp
  src/everest_api_types/system/json_codec.cpp
  src/everest_api_types/system/wrapper.cpp

  src/everest_api_types/text_message/codec.cpp
  src/everest_api_types/text_message/json_codec.cpp
  src/everest_api_types/text_message/wrapper.cpp

  src/everest_api_types/uk_random_delay/codec.cpp
  src/everest_api_types/uk_random_delay/json_codec.cpp
  src/everest_api_types/uk_random_delay/wrapper.cpp

  src/everest_api_types/generic/codec.cpp
  src/everest_api_types/generic/json_codec.cpp
  src/everest_api_types/generic/string.cpp

  src/everest_api_types/utilities/codec.cpp
  src/everest_api_types/utilities/Topics.cpp
)

target_link_libraries(everest_api_types
  PRIVATE nlohmann_json::nlohmann_json
)

install(TARGETS everest_api_types
  EXPORT everest_api_types-targets
  LIBRARY
)

install(
  DIRECTORY include/everest_api_types
  DESTINATION include
)

evc_setup_package(
  NAME everest-api-types
  EXPORT everest_api_types-targets
  NAMESPACE everest
)

if(${BUILD_TESTING})
    add_subdirectory(tests)
endif()

