add_library(slac)
add_library(slac::slac ALIAS slac)

target_include_directories(slac PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_sources(slac
    PRIVATE
        src/channel.cpp
        src/slac.cpp
        src/packet_socket.cpp
)

target_link_libraries(slac
    PRIVATE
        everest::tls
)

add_subdirectory(io)
add_subdirectory(fsm/ev)
add_subdirectory(fsm/evse)

if(BUILD_DEV_TESTS)
  add_subdirectory(test)
endif()

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
