
target_sources(ocpp
    PRIVATE
        websocket_base.cpp
        websocket_uri.cpp        
        websocket.cpp
        websocket_libwebsockets.cpp

        # TODO: remove when interface refactor is done
        websocket_plain.cpp
        websocket_tls.cpp
)
