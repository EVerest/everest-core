# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

add_subdirectory(grpc_libs)
add_custom_target(eebus_tests ALL)
add_dependencies(eebus_tests eebus_grpc_api_create_cert)

get_target_property(CREATE_CERT_BINARY_FILE eebus_grpc_api_create_cert TARGET_FILE)

add_dependencies(eebus_tests 
    eebus_tests_certs_ems
    eebus_tests_certs_evse
)
