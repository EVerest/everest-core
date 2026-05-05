# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

set(GENERATED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/generated")

# setup_grpc() is already called from modules/EnergyManagement/EEBUS/grpc_libs/
# and its imported targets (gRPC::grpc++) are global. Re-invoking the macro with
# GRPC_EDM=ON would try to create the ALIAS a second time and fail, so guard it.
# _GRPC_SETUP_DONE is a per-scope variable gating setup_grpc_generator below;
# set it explicitly here because cross-subdir scopes don't inherit it.
if (NOT TARGET gRPC::grpc++)
    setup_grpc()
endif()
set(_GRPC_SETUP_DONE TRUE)

if (ENABLE_GRPC_GENERATOR)
    if (NOT CPM_PACKAGE_eebus-grpc_SOURCE_DIR)
        message(FATAL_ERROR "CPM_PACKAGE_eebus-grpc_SOURCE_DIR not set, it is required becaue it provides the protobuf files")
    endif()
    get_filename_component(PROTOBUF_DIR
        "${CPM_PACKAGE_eebus-grpc_SOURCE_DIR}/protobuf/"
        ABSOLUTE
    )

    setup_grpc_generator()

    generate_py_from_proto(
        TARGET_NAME generate_proto_common_types_py
        PROTOBUF_DIR ${PROTOBUF_DIR}
        OUT_DIR ${GENERATED_DIR}
        PROTO_FILES
            "${PROTOBUF_DIR}/common_types/types.proto"
    )

    generate_py_from_proto(
        TARGET_NAME generate_proto_control_service_py
        PROTOBUF_DIR ${PROTOBUF_DIR}
        OUT_DIR ${GENERATED_DIR}
        PROTO_FILES
            "${PROTOBUF_DIR}/control_service/control_service.proto"
            "${PROTOBUF_DIR}/control_service/messages.proto"
            "${PROTOBUF_DIR}/control_service/types.proto"
    )

    generate_py_from_proto(
        TARGET_NAME generate_proto_cs_lpc_service_py
        PROTOBUF_DIR ${PROTOBUF_DIR}
        OUT_DIR ${GENERATED_DIR}
        PROTO_FILES
            "${PROTOBUF_DIR}/usecases/cs/lpc/service.proto"
            "${PROTOBUF_DIR}/usecases/cs/lpc/messages.proto"
    )

    add_custom_target(generate_eebus_grpc_api_proto_py
        ALL
        DEPENDS
            generate_proto_common_types_py
            generate_proto_control_service_py
            generate_proto_cs_lpc_service_py
    )
endif()
