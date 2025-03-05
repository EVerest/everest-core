# This macro generates the C++ files from the protobuf files
# provided by the eebus-grpc-api package
# The targets generate_proto_common_types, 
# generate_proto_control_service and generate_proto_cs_lpc_service
# are added to allow manual generation of the files
macro(generate_cpp_from_protobuf)
    if (NOT CPM_PACKAGE_eebus-grpc-api_SOURCE_DIR)
        message(FATAL_ERROR "CPM_PACKAGE_eebus-grpc-api_SOURCE_DIR not set, it is required becaue it provides the protobuf files")
    endif()
    get_filename_component(PROTOBUF_DIR
        "${CPM_PACKAGE_eebus-grpc-api_SOURCE_DIR}/protobuf/"
        ABSOLUTE
    )

    setup_grpc_generator()

    generate_cpp_from_proto(
        TARGET_NAME generate_proto_common_types
        PROTOBUF_DIR ${PROTOBUF_DIR}
        OUT_DIR ${GENERATED_DIR}
        HDRS_VAR COMMON_TYPES_HDRS
        SRCS_VAR COMMON_TYPES_SRCS
        PROTO_FILES
            "${PROTOBUF_DIR}/common_types/types.proto"
    )

    generate_cpp_from_proto(
        TARGET_NAME generate_proto_control_service
        PROTOBUF_DIR ${PROTOBUF_DIR}
        OUT_DIR ${GENERATED_DIR}
        HDRS_VAR CONTROL_SERVICE_HDRS
        SRCS_VAR CONTROL_SERVICE_SRCS
        PROTO_FILES
            "${PROTOBUF_DIR}/control_service/control_service.proto"
            "${PROTOBUF_DIR}/control_service/messages.proto"
            "${PROTOBUF_DIR}/control_service/types.proto"
    )

    generate_cpp_from_proto(
        TARGET_NAME generate_proto_cs_lpc_service
        PROTOBUF_DIR ${PROTOBUF_DIR}
        OUT_DIR ${GENERATED_DIR}
        HDRS_VAR CS_LPC_SERVICE_HDRS
        SRCS_VAR CS_LPC_SERVICE_SRCS
        PROTO_FILES
            "${PROTOBUF_DIR}/usecases/cs/lpc/service.proto"
            "${PROTOBUF_DIR}/usecases/cs/lpc/messages.proto"
    )

    add_custom_target(generate_eebus_grpc_api_proto
        DEPENDS
            generate_proto_common_types
            generate_proto_control_service
            generate_proto_cs_lpc_service
    )
endmacro()
