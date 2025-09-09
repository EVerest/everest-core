include(Catch)

function(create_states_test_target NAME)
    add_executable(test_${NAME} ${NAME}.cpp)
    target_link_libraries(test_${NAME}
        PRIVATE
            iso15118
            Catch2::Catch2WithMain
    )
    catch_discover_tests(test_${NAME})
endfunction()

create_states_test_target(ac_charge_loop)
create_states_test_target(ac_charge_parameter_discovery)
create_states_test_target(authorization_setup)
create_states_test_target(authorization)
create_states_test_target(service_discovery)
create_states_test_target(service_detail)
create_states_test_target(service_selection)
create_states_test_target(dc_charge_parameter_discovery)
create_states_test_target(schedule_exchange)
create_states_test_target(dc_cable_check)
create_states_test_target(dc_pre_charge)
create_states_test_target(power_delivery)
create_states_test_target(dc_charge_loop)
create_states_test_target(dc_welding_detection)
create_states_test_target(session_stop)
