#
# AUTO GENERATED - MARKED REGIONS WILL BE KEPT
# template version 3
#

# module setup:
#   - ${MODULE_NAME}: module name
ev_setup_cpp_module()

# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1
# insert your custom targets and additional config variables here
find_package(OpenSSL)

target_link_libraries(${MODULE_NAME}
    PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
        everest::util
        everest::ocpp
        everest::ocpp_evse_security
        everest::ocpp_conversions
        everest::external_energy_limits
)

target_compile_options(${MODULE_NAME}
    PRIVATE
        -Wimplicit-fallthrough
        -Werror=switch-enum
)
# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1

target_sources(${MODULE_NAME}
    PRIVATE
        "auth_validator/auth_token_validatorImpl.cpp"
        "auth_provider/auth_token_providerImpl.cpp"
        "data_transfer/ocpp_data_transferImpl.cpp"
        "ocpp_generic/ocppImpl.cpp"
        "session_cost/session_costImpl.cpp"
)

# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
# insert other things like install cmds etc here
target_sources(${MODULE_NAME}
    PRIVATE
        "conversions.cpp"
        "transaction_handler.cpp"
        "device_model/everest_device_model_storage.cpp"
        "device_model/composed_device_model_storage.cpp"
        "device_model/definitions.cpp"
)

if(EVEREST_CORE_BUILD_TESTING)
    add_subdirectory(tests)
endif()
# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
