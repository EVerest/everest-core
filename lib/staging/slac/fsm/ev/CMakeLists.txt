add_library(slac_fsm_ev)
add_library(slac::fsm::ev ALIAS slac_fsm_ev)
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

if(EVEREST_CORE_BUILD_TESTING)
    append_coverage_compiler_flags_to_target(slac_fsm_ev)
endif()
