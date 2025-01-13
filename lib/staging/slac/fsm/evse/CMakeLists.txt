add_library(slac_fsm_evse)
add_library(slac::fsm::evse ALIAS slac_fsm_evse)

target_include_directories(slac_fsm_evse
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(slac_fsm_evse
    PRIVATE
        src/misc.cpp
        src/context.cpp
        src/states/others.cpp
        src/states/matching.cpp
        src/states/matching_handle_slac.cpp
)

target_link_libraries(slac_fsm_evse
    PUBLIC
        slac::slac
        fsm::fsm
)

if(EVEREST_CORE_BUILD_TESTING)
    append_coverage_compiler_flags_to_target(slac_fsm_evse)
endif()
