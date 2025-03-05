# Adds the library proto_common_types
# Sets the include directories and links the dependencies
macro(_add_proto_common_types_lib)
    add_library(proto_common_types
        ${GENERATED_DIR}/common_types/types.pb.h
        ${GENERATED_DIR}/common_types/types.pb.cc
        ${GENERATED_DIR}/common_types/types.grpc.pb.h
        ${GENERATED_DIR}/common_types/types.grpc.pb.cc
        ${GENERATED_DIR}/common_types/types.grpc-ext.pb.h
        ${GENERATED_DIR}/common_types/types.grpc-ext.pb.cc
    )
    target_include_directories(proto_common_types
        PUBLIC
            ${GENERATED_DIR}
    )
    target_link_libraries(proto_common_types
        PUBLIC
            ${COMMON_DEPS}
    )
endmacro()

# Adds the library proto_control_service
# Sets the include directories and links the dependencies
macro(_add_proto_control_service_lib)
    add_library(proto_control_service
        ${GENERATED_DIR}/control_service/control_service.pb.h
        ${GENERATED_DIR}/control_service/control_service.pb.cc
        ${GENERATED_DIR}/control_service/control_service.grpc.pb.h
        ${GENERATED_DIR}/control_service/control_service.grpc.pb.cc
        ${GENERATED_DIR}/control_service/control_service.grpc-ext.pb.h
        ${GENERATED_DIR}/control_service/control_service.grpc-ext.pb.cc
        ${GENERATED_DIR}/control_service/messages.pb.h
        ${GENERATED_DIR}/control_service/messages.pb.cc
        ${GENERATED_DIR}/control_service/messages.grpc.pb.h
        ${GENERATED_DIR}/control_service/messages.grpc.pb.cc
        ${GENERATED_DIR}/control_service/messages.grpc-ext.pb.h
        ${GENERATED_DIR}/control_service/messages.grpc-ext.pb.cc
        ${GENERATED_DIR}/control_service/types.pb.h
        ${GENERATED_DIR}/control_service/types.pb.cc
        ${GENERATED_DIR}/control_service/types.grpc.pb.h
        ${GENERATED_DIR}/control_service/types.grpc.pb.cc
        ${GENERATED_DIR}/control_service/types.grpc-ext.pb.h
        ${GENERATED_DIR}/control_service/types.grpc-ext.pb.cc
    )
    target_include_directories(proto_control_service
        PUBLIC
            ${GENERATED_DIR}
    )
    target_link_libraries(proto_control_service
        PUBLIC
            proto_common_types
            ${COMMON_DEPS}
    )
endmacro()

# Adds the library proto_cs_lpc_service
# Sets the include directories and links the dependencies
macro(_add_proto_cs_lpc_service_lib)
    add_library(proto_cs_lpc_service
        ${GENERATED_DIR}/usecases/cs/lpc/service.pb.h
        ${GENERATED_DIR}/usecases/cs/lpc/service.pb.cc
        ${GENERATED_DIR}/usecases/cs/lpc/service.grpc.pb.h
        ${GENERATED_DIR}/usecases/cs/lpc/service.grpc.pb.cc
        ${GENERATED_DIR}/usecases/cs/lpc/service.grpc-ext.pb.h
        ${GENERATED_DIR}/usecases/cs/lpc/service.grpc-ext.pb.cc
        ${GENERATED_DIR}/usecases/cs/lpc/messages.pb.h
        ${GENERATED_DIR}/usecases/cs/lpc/messages.pb.cc
        ${GENERATED_DIR}/usecases/cs/lpc/messages.grpc.pb.h
        ${GENERATED_DIR}/usecases/cs/lpc/messages.grpc.pb.cc
        ${GENERATED_DIR}/usecases/cs/lpc/messages.grpc-ext.pb.h
        ${GENERATED_DIR}/usecases/cs/lpc/messages.grpc-ext.pb.cc
    )
    target_include_directories(proto_cs_lpc_service
        PUBLIC
            ${GENERATED_DIR}
    )
    target_link_libraries(proto_cs_lpc_service
        PUBLIC
            proto_common_types
            ${COMMON_DEPS}
    )
endmacro()

# This macro adds the following libraries:
#   - proto_common_types
#   - proto_control_service
#   - proto_cs_lpc_service
macro(add_libraries)
    find_package(absl CONFIG REQUIRED)
    set(COMMON_DEPS
        absl::absl_log
        gRPC::grpc++
    )

    _add_proto_common_types_lib()
    _add_proto_control_service_lib()
    _add_proto_cs_lpc_service_lib()
endmacro()
