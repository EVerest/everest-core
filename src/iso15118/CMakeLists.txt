find_package(Threads REQUIRED)

add_library(iso15118)
add_library(iso15118::iso15118 ALIAS iso15118)
target_include_directories(iso15118
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

target_sources(iso15118
    PRIVATE
        misc/helper.cpp
        misc/cb_exi.cpp

        io/connection_ssl.cpp
        io/connection_plain.cpp
        io/logging.cpp
        io/poll_manager.cpp
        io/sdp_packet.cpp
        io/sdp_server.cpp
        io/socket_helper.cpp

        session/feedback.cpp
        session/iso.cpp
        session/logger.cpp
        
        d20/fsm.cpp
        d20/context.cpp
        d20/context_helper.cpp
        d20/control_event_queue.cpp
        d20/session.cpp
        d20/config.cpp

        d20/state/supported_app_protocol.cpp
        d20/state/session_setup.cpp
        d20/state/authorization_setup.cpp
        d20/state/authorization.cpp
        d20/state/service_discovery.cpp
        d20/state/service_detail.cpp
        d20/state/service_selection.cpp
        d20/state/dc_charge_parameter_discovery.cpp
        d20/state/schedule_exchange.cpp
        d20/state/dc_cable_check.cpp
        d20/state/dc_pre_charge.cpp
        d20/state/power_delivery.cpp
        d20/state/dc_charge_loop.cpp
        d20/state/dc_welding_detection.cpp
        d20/state/session_stop.cpp

        message/variant.cpp
        message/supported_app_protocol.cpp
        message/session_setup.cpp
        message/common.cpp
        message/authorization_setup.cpp
        message/authorization.cpp
        message/service_discovery.cpp
        message/service_detail.cpp
        message/service_selection.cpp
        message/dc_charge_parameter_discovery.cpp
        message/schedule_exchange.cpp
        message/dc_cable_check.cpp
        message/dc_pre_charge.cpp
        message/power_delivery.cpp
        message/dc_charge_loop.cpp
        message/dc_welding_detection.cpp
        message/session_stop.cpp
        message/ac_charge_parameter_discovery.cpp
        message/ac_charge_loop.cpp

        tbd_controller.cpp    
)

if (NOT ISO15118_LINK_CUSTOM_MBEDTLS)
    set(MBEDTLS_LINK_DEPENDENCIES
        mbedtls
        mbedx509
        mbedcrypto
    )
else ()
    set(MBEDTLS_LINK_DEPENDENCIES
        mbedtls_custom
        mbedx509_custom
        mbedcrypto_custom
    )
endif ()

target_link_libraries(iso15118
    PUBLIC
        # FIXME (aw): would be nice if we could make this private!
        fsm::fsm
        cbv2g::tp
        cbv2g::iso20
    PRIVATE
        Threads::Threads

        ${MBEDTLS_LINK_DEPENDENCIES}

)

# FIXME (aw): do we want to have this public here?
target_compile_features(iso15118 PUBLIC cxx_std_17)
