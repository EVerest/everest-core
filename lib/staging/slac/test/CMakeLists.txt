add_executable(evse_slac_test)
target_sources(evse_slac_test
    PRIVATE
        evse_slac_test.cpp
)

target_link_libraries(evse_slac_test
    PRIVATE
        slac_fsm_evse
        fmt::fmt
)

add_executable(evse_vs_ev)
target_sources(evse_vs_ev
    PRIVATE
        evse_vs_ev/socket_pair_bridge.cpp
        evse_vs_ev/plc_emu.cpp
        evse_vs_ev/main.cpp
)

target_link_libraries(evse_vs_ev
    PRIVATE
        slac::fsm::evse
        slac::fsm::ev
        Threads::Threads
)

add_executable(bridger)
target_sources(bridger
    PRIVATE
        evse_vs_ev/plc_emu.cpp
        bridger.cpp
)
target_link_libraries(bridger
    PRIVATE
        slac::slac
        fmt::fmt
)
