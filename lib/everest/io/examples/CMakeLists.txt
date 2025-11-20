add_executable(mosquitto_client test_mosquitto_client.cpp)

target_link_libraries(mosquitto_client
    PRIVATE
        everest::io
)

target_compile_features(mosquitto_client PUBLIC cxx_std_17)
target_compile_options(mosquitto_client PRIVATE -fsanitize=address -g)
target_link_options(mosquitto_client PRIVATE -static-libasan -fsanitize=address)


add_executable(test_serial test_serial.cpp)

target_link_libraries(test_serial
  PRIVATE
  everest::io
)

target_compile_features(test_serial PUBLIC cxx_std_17)
target_compile_options(test_serial PRIVATE -fsanitize=address -g)
target_link_options(test_serial PRIVATE -static-libasan -fsanitize=address)

###

add_executable(test_socket_can test_socket_can.cpp)

target_link_libraries(test_socket_can
  PRIVATE
  everest::io
)

target_compile_features(test_socket_can PUBLIC cxx_std_17)
target_compile_options(test_socket_can PRIVATE -fsanitize=address -g)
target_link_options(test_socket_can PRIVATE -static-libasan -fsanitize=address)

###

add_executable(test_tcp_client test_tcp_client.cpp)

target_link_libraries(test_tcp_client
  PRIVATE
  everest::io
)

target_compile_features(test_tcp_client PUBLIC cxx_std_17)
target_compile_options(test_tcp_client PRIVATE -fsanitize=address -g)
target_link_options(test_tcp_client PRIVATE -static-libasan -fsanitize=address)

###

add_executable(test_tun_tap test_tun_tap.cpp)

target_link_libraries(test_tun_tap
  PRIVATE
  everest::io
)

target_compile_features(test_tun_tap PUBLIC cxx_std_17)
target_compile_options(test_tun_tap PRIVATE -fsanitize=address -g)
target_link_options(test_tun_tap PRIVATE -static-libasan -fsanitize=address)

###

add_executable(test_udp_client test_udp_client.cpp)

target_link_libraries(test_udp_client
    PRIVATE
        everest::io
)

target_compile_features(test_udp_client PUBLIC cxx_std_17)
target_compile_options(test_udp_client PRIVATE -fsanitize=address -g)
target_link_options(test_udp_client PRIVATE -static-libasan -fsanitize=address)

###

add_executable(test_udp_server test_udp_server.cpp)

target_link_libraries(test_udp_server
    PRIVATE
        everest::io
)

target_compile_features(test_udp_server PUBLIC cxx_std_17)
target_compile_options(test_udp_server PRIVATE -fsanitize=address -g)
target_link_options(test_udp_server PRIVATE -static-libasan -fsanitize=address)














