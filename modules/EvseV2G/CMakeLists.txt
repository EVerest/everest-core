#
# AUTO GENERATED - MARKED REGIONS WILL BE KEPT
# template version 3
#

# module setup:
#   - ${MODULE_NAME}: module name
ev_setup_cpp_module()

# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1
# insert your custom targets and additional config variables here
option(USING_MBED_TLS "Use MbedTLS for V2G" OFF)

if(USING_MBED_TLS)
target_compile_definitions(${MODULE_NAME} PRIVATE
    EVEREST_MBED_TLS
)
endif()
# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1

target_sources(${MODULE_NAME}
    PRIVATE
        "charger/ISO15118_chargerImpl.cpp"
)

# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
# Add pkg-config functionality
find_package(PkgConfig REQUIRED)

# search for libevent.pc
pkg_search_module(EVENT REQUIRED libevent)

target_include_directories(${MODULE_NAME} PRIVATE
    crypto
    connection
)

target_link_libraries(${MODULE_NAME} PUBLIC ${EVENT_LIBRARIES} -levent -lpthread -levent_pthreads)

target_link_libraries(${MODULE_NAME}
    PRIVATE
        cbv2g::din
        cbv2g::iso2
        cbv2g::tp
)

target_sources(${MODULE_NAME}
    PRIVATE
        "connection/connection.cpp"
        "iso_server.cpp"
        "din_server.cpp"
        "log.cpp"
        "sdp.cpp"
        "tools.cpp"
        "v2g_ctx.cpp"
        "v2g_server.cpp"
)

if(USING_MBED_TLS)
# needed for header file enum definition
target_include_directories(${MODULE_NAME} PRIVATE
        ../../lib/staging/tls ../../lib/staging/util
)
target_link_libraries(${MODULE_NAME}
    PRIVATE
        mbedcrypto
        mbedtls
        mbedx509
)
target_sources(${MODULE_NAME}
    PRIVATE
        "crypto/crypto_mbedtls.cpp"
)
else()
target_link_libraries(${MODULE_NAME}
    PRIVATE
        everest::tls
)
target_sources(${MODULE_NAME}
    PRIVATE
        "crypto/crypto_openssl.cpp"
        "connection/tls_connection.cpp"
)
endif()

if(EVEREST_CORE_BUILD_TESTING)
    add_subdirectory(tests)
endif()
# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
