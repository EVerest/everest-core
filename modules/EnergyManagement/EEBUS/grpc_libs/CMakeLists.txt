include(${CMAKE_CURRENT_LIST_DIR}/cmake/generate-cpp-from-protobuf.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/sanity-checks.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/add-libraries.cmake)

set(GENERATED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/generated")

setup_grpc()

if (ENABLE_GRPC_GENERATOR)
    generate_cpp_from_protobuf()
endif()

add_libraries()

if (ENABLE_GRPC_GENERATOR)
    # Force regeneration, if ENABLE_GRPC_GENERATOR is set
    add_dependencies(proto_common_types generate_eebus_grpc_api_proto)
    add_dependencies(proto_control_service generate_eebus_grpc_api_proto)
    add_dependencies(proto_cs_lpc_service generate_eebus_grpc_api_proto)

    sanity_checks()
endif()
