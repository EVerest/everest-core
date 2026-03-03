include(Catch)

function(create_ev_fsm_test_target NAME)
    add_executable(test_ev_fsm_${NAME} ${NAME}.cpp)
    target_sources(test_ev_fsm_${NAME}
        PRIVATE
            helper.cpp
    )
    target_link_libraries(test_ev_fsm_${NAME}
        PRIVATE
            iso15118
            Catch2::Catch2WithMain
    )
    catch_discover_tests(test_ev_fsm_${NAME})
endfunction()

create_ev_fsm_test_target(session_setup)
create_ev_fsm_test_target(authorization_setup)
create_ev_fsm_test_target(authorization)
