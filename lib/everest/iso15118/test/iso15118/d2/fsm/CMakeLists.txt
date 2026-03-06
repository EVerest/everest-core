include(Catch)

function(create_fsm_test_target NAME)
    add_executable(test_fsm_d2_${NAME} ${NAME}.cpp)
    target_sources(test_fsm_d2_${NAME}
        PRIVATE
            helper.cpp
    )
    target_link_libraries(test_fsm_d2_${NAME}
        PRIVATE
            iso15118
            Catch2::Catch2WithMain
    )
    catch_discover_tests(test_fsm_d2_${NAME})
endfunction()

create_fsm_test_target(service_discovery)
create_fsm_test_target(session_setup)

