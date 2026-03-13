add_library(slac_event)
add_library(slac::event ALIAS slac_event)
ev_register_library_target(slac_event)

target_include_directories(slac_event
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(slac_event
    PRIVATE
    src/slac_event.cpp
    src/slac_socket.cpp
)

target_link_libraries(slac_event
    PUBLIC
        everest::io
        slac::slac
)
