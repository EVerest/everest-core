
target_sources(ocpp
    PRIVATE
        websocket_base.cpp
        websocket_uri.cpp        
        websocket.cpp
        websocket_libwebsockets.cpp    
)

if(LIBOCPP_ENABLE_DEPRECATED_WEBSOCKETPP)
    target_sources(ocpp
        PRIVATE    
            websocket_plain.cpp
            websocket_tls.cpp
    )
endif()
