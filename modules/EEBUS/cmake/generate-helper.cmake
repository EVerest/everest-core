find_package(absl CONFIG REQUIRED)

function(generate_cpp_from_proto)
    set(options)
    set(oneValueArgs
        OUT_DIR
        PROTOBUF_DIR
        LIBRARY_NAME
    )
    set(multiValueArgs
        PROTO_FILES
    )
    cmake_parse_arguments(arg
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    set(OUTPUT_FILES)

    # for each proto files add 6 outfiles .pb.h, .pb.cc, .grpc.pb.h, .grpc.pb.cc, .grpc-ext.pb.h, .grpc-ext.pb.cc
    foreach(proto_file ${arg_PROTO_FILES})
        file(RELATIVE_PATH proto_name ${arg_PROTOBUF_DIR} ${proto_file})
        string(REPLACE ".proto" "" proto_name ${proto_name})
        list(APPEND OUTPUT_FILES
            "${arg_OUT_DIR}/${proto_name}.pb.h"
            "${arg_OUT_DIR}/${proto_name}.pb.cc"
            "${arg_OUT_DIR}/${proto_name}.grpc.pb.h"
            "${arg_OUT_DIR}/${proto_name}.grpc.pb.cc"
            "${arg_OUT_DIR}/${proto_name}.grpc-ext.pb.h"
            "${arg_OUT_DIR}/${proto_name}.grpc-ext.pb.cc"
        )
    endforeach()

    set(COMMAND_ARGS)
    list(APPEND COMMAND_ARGS
        -I
        "${arg_PROTOBUF_DIR}"
    )

    # standard protobuf cpp
    list(APPEND COMMAND_ARGS
        --cpp_out
        "${arg_OUT_DIR}"
    )

    # grpc cpp
    # list(APPEND COMMAND_ARGS
    # --grpc_out
    # "${arg_OUT_DIR}"
    # --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
    # )
    # extended grpc cpp
    list(APPEND COMMAND_ARGS
        --ext_grpc_out
        "${arg_OUT_DIR}"
        --plugin=protoc-gen-ext_grpc="$<TARGET_FILE:grpc_extended_cpp_plugin>"
    )

    # proto files
    list(APPEND COMMAND_ARGS
        ${arg_PROTO_FILES}
    )

    add_custom_command(
        OUTPUT
            ${OUTPUT_FILES}
        COMMAND
            mkdir -p ${arg_OUT_DIR}
        COMMAND
            ${_PROTOBUF_PROTOC}
        ARGS
            ${COMMAND_ARGS}
    )

    add_library(${arg_LIBRARY_NAME}
        ${OUTPUT_FILES}
    )
    target_link_libraries(${arg_LIBRARY_NAME}
        PUBLIC
            absl::absl_log
            ${_REFLECTION}
            ${_GRPC_GRPCPP}
            ${_PROTOBUF_LIBPROTOBUF}
    )
    target_include_directories(${arg_LIBRARY_NAME}
        PUBLIC
            "${arg_OUT_DIR}"
    )
endfunction()
