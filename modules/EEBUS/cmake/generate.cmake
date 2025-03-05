include(cmake/generate-helper.cmake)

get_filename_component(PROTOBUF_DIR
    "${generic-eebus_SOURCE_DIR}/protobuf/"
    ABSOLUTE
)
set(OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")

generate_cpp_from_proto(
    LIBRARY_NAME "proto_common_types"
    PROTOBUF_DIR "${PROTOBUF_DIR}"
    OUT_DIR "${OUT_DIR}"
    PROTO_FILES
        "${PROTOBUF_DIR}/common_types/types.proto"
)

generate_cpp_from_proto(
    LIBRARY_NAME "proto_control_service"
    PROTOBUF_DIR "${PROTOBUF_DIR}"
    OUT_DIR "${OUT_DIR}"
    PROTO_FILES
        "${PROTOBUF_DIR}/control_service/control_service.proto"
        "${PROTOBUF_DIR}/control_service/messages.proto"
        "${PROTOBUF_DIR}/control_service/types.proto"
)
target_link_libraries(proto_control_service
    PUBLIC
        proto_common_types
)

generate_cpp_from_proto(
    LIBRARY_NAME "proto_cs_lpc_service"
    PROTOBUF_DIR "${PROTOBUF_DIR}"
    OUT_DIR "${OUT_DIR}"
    PROTO_FILES
        "${PROTOBUF_DIR}/usecases/cs/lpc/service.proto"
        "${PROTOBUF_DIR}/usecases/cs/lpc/messages.proto"
)
target_link_libraries(proto_cs_lpc_service
    PUBLIC
        proto_common_types
)
