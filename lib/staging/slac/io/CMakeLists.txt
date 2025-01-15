add_library(slac_io)
add_library(slac::io ALIAS slac_io)

target_include_directories(slac_io
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(slac_io
    PRIVATE
        src/io.cpp
)

target_link_libraries(slac_io
    PUBLIC
        slac::slac
)

ev_register_library_target(slac_io)
