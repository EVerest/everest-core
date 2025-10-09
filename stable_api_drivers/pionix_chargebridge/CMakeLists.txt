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

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_executable(pionix_chargebridge_tool
  src/crypto/openssl_crypto_supplier.cpp

  src/evse_bsp/evse_bsp.cpp

  src/firmware_update/sync_fw_updater.cpp

  src/utilities/filesystem.cpp
  src/utilities/logging.cpp
  src/utilities/parse_config.cpp
  src/utilities/string.cpp
  src/utilities/symlink.cpp
  src/utilities/sync_udp_client.cpp

  src/can_bridge.cpp
  src/charge_bridge.cpp
  src/evse_bridge.cpp
  src/gpio_bridge.cpp
  src/heartbeat_service.cpp
  src/plc_bridge.cpp
  src/serial_bridge.cpp

  main.cpp
)

target_link_libraries(pionix_chargebridge_tool
  everest::io
  everest::everest_api_types
  nlohmann_json::nlohmann_json
  yaml-cpp::yaml-cpp

  OpenSSL::SSL
  OpenSSL::Crypto
)

target_include_directories(pionix_chargebridge_tool
  PRIVATE include
  PRIVATE shared
)

set(cb_firmware_binary firmware/charge-bridge-fw_complete.cbfw)

add_custom_command(
  TARGET pionix_chargebridge_tool
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  "${CMAKE_CURRENT_SOURCE_DIR}/${cb_firmware_binary}"
  "$<TARGET_FILE_DIR:pionix_chargebridge_tool>/"
  COMMENT "Copying Pionix ChargeBridge firmware binary..."
)

# target_compile_options(pionix_chargebridge_tool PRIVATE -fsanitize=address -g)
# target_link_options(pionix_chargebridge_tool PRIVATE -static-libasan -fsanitize=address)

install (TARGETS pionix_chargebridge_tool)
install (FILES "${CMAKE_CURRENT_SOURCE_DIR}/${cb_firmware_binary}" DESTINATION ${CMAKE_INSTALL_DATADIR}/pionix/chargebridge/firmware)
