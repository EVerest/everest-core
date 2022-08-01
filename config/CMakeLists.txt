generate_config_run_script(CONFIG sil)
generate_config_run_script(CONFIG sil-two-evse)
generate_config_run_script(CONFIG hil)
generate_config_run_script(CONFIG auth)

generate_config_run_script(
    CONFIG sil-ocpp
    OUTPUT sil-ocpp-complete
)
generate_config_run_script(
    CONFIG sil-ocpp
    OUTPUT sil-ocpp-manager
    ADDITIONAL_ARGUMENTS
        --standalone charge_point
)
generate_config_run_script(
    CONFIG sil-ocpp
    OUTPUT sil-ocpp-module
    ADDITIONAL_ARGUMENTS
        --module charge_point
)
