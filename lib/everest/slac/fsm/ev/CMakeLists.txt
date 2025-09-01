add_library(slac_fsm_ev)
add_library(slac::fsm::ev ALIAS slac_fsm_ev)
ev_register_library_target(slac_fsm_ev)

target_include_directories(slac_fsm_ev
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(slac_fsm_ev
    PRIVATE
        src/context.cpp
        src/states/others.cpp
        src/states/sounding.cpp
)

target_link_libraries(slac_fsm_ev
    PUBLIC
        slac::slac
        fsm::fsm
)
