
target_sources(ocpp
    PRIVATE
        websocket_base.cpp
        websocket_uri.cpp
        websocket_plain.cpp
        websocket_tls.cpp
        websocket.cpp
)

if(LIBOCPP_ENABLE_LIBWEBSOCKETS)
    target_sources(ocpp
        PRIVATE
            websocket_tls_tpm.cpp
    )
endif()
