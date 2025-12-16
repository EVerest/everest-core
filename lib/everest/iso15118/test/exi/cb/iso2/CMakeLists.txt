include(Catch)

function(create_exi_test_target NAME)
    add_executable(test_exi_d2_${NAME} ${NAME}.cpp)
    target_link_libraries(test_exi_d2_${NAME}
        PRIVATE
            iso15118
            Catch2::Catch2WithMain
    )
    catch_discover_tests(test_exi_d2_${NAME})
endfunction()

create_exi_test_target(dc_cable_check)
create_exi_test_target(dc_current_demand)
create_exi_test_target(dc_pre_charge)
create_exi_test_target(dc_welding_detection)
create_exi_test_target(service_discovery)
create_exi_test_target(session_setup)
create_exi_test_target(authorization)

