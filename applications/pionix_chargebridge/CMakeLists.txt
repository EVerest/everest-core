cmake_minimum_required(VERSION 3.22)

# include(FetchContent)
# FetchContent_Declare(
#   nlohmann_json
#   GIT_REPOSITORY git@github.com:nlohmann/json.git
#   GIT_TAG        v3.12.0
# )
# FetchContent_MakeAvailable(nlohmann_json)

# FetchContent_Declare(
#   yaml-cpp
#   GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
#   GIT_TAG master
# )
# FetchContent_MakeAvailable(yaml-cpp)

find_package(OpenSSL 3 REQUIRED)
find_package(ryml QUIET)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_executable(pionix_chargebridge
  src/crypto/openssl_crypto_supplier.cpp

  src/everest_api/api_connector.cpp
  src/everest_api/evse_bsp_api.cpp
  src/everest_api/ovm_api.cpp

  src/firmware_update/sync_fw_updater.cpp

  src/utilities/filesystem.cpp
  src/utilities/logging.cpp
  src/utilities/parse_config.cpp
  src/utilities/print_config.cpp
  src/utilities/string.cpp
  src/utilities/symlink.cpp
  src/utilities/sync_udp_client.cpp
  src/utilities/type_converters.cpp

  src/can_bridge.cpp
  src/charge_bridge.cpp
  src/evse_bridge.cpp
  src/gpio_bridge.cpp
  src/heartbeat_service.cpp
  src/plc_bridge.cpp
  src/serial_bridge.cpp

  main.cpp
)

target_link_libraries(pionix_chargebridge
  everest::io
  everest::everest_api_types
  nlohmann_json::nlohmann_json
  ryml::ryml

  OpenSSL::SSL
  OpenSSL::Crypto
)

target_include_directories(pionix_chargebridge
  PRIVATE include
  PRIVATE shared
)

set(cb_firmware_binary config/firmware/charge-bridge-fw_complete.cbfw)

add_custom_command(
  TARGET pionix_chargebridge
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  "${CMAKE_CURRENT_SOURCE_DIR}/${cb_firmware_binary}"
  "$<TARGET_FILE_DIR:pionix_chargebridge>/"
  COMMENT "Copying Pionix ChargeBridge firmware binary..."
)

# target_compile_options(pionix_chargebridge PRIVATE -fsanitize=address -g)
# target_link_options(pionix_chargebridge PRIVATE -static-libasan -fsanitize=address)

install (TARGETS pionix_chargebridge)
install (FILES "${CMAKE_CURRENT_SOURCE_DIR}/${cb_firmware_binary}" DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/chargebridge/firmware)
install (FILES "${CMAKE_CURRENT_SOURCE_DIR}/config/config-CB-EVAL.yaml" DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/chargebridge)
install (FILES "${CMAKE_CURRENT_SOURCE_DIR}/config/config-CB-SAT-AC.yaml" DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/chargebridge)
