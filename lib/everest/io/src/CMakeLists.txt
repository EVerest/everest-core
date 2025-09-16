
add_library(everest_io)
add_library(everest::io ALIAS everest_io)

target_sources(everest_io
    PRIVATE
        event/fd_event_handler.cpp
        event/unique_fd.cpp
        event/event_fd.cpp
        event/timer_fd.cpp
        can/socket_can_handler.cpp
        can/can_payload.cpp
        socket/socket.cpp
        utilities/generic_error_state.cpp
        event/fd_event_client.cpp
        udp/udp_socket.cpp
        udp/udp_payload.cpp
        utilities/stop_watch.cpp
        serial/serial.cpp
        serial/pty_handler.cpp
        serial/event_pty.cpp
        mqtt/mqtt_handler.cpp
        mqtt/mqtt_client.cpp
        tun_tap/tap_handler.cpp
        tcp/tcp_socket.cpp
        netlink/vcan_netlink_manager.cpp
)

target_include_directories(everest_io
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(everest_io
    PUBLIC
        mqttc
)

target_compile_features(everest_io PUBLIC cxx_std_17)

target_compile_options(everest_io
    PRIVATE
        -Wall
        -Wextra
        -Wshadow
        -Wnon-virtual-dtor
        -Wunused
        -Wpedantic
        -Wformat=2
        -Werror
)

include(CMakePackageConfigHelpers)

set_target_properties(everest_io
    PROPERTIES EXPORT_NAME io
)

install(
  TARGETS everest_io mqttc
  EXPORT everest_io-targets
)

evc_setup_package(
  NAME everest-io
  EXPORT everest_io-targets
  NAMESPACE everest
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION include
)
