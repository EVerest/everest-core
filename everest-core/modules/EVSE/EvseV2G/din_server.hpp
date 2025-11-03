// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2023 chargebyte GmbH
// Copyright (C) 2023 Contributors to EVerest

#ifndef DIN_SERVER_HPP
#define DIN_SERVER_HPP

#include "v2g.hpp"

/*!
 * \brief The din_state_id enum
 */
enum din_state_id {
    WAIT_FOR_SESSIONSETUP = 0,
    WAIT_FOR_SERVICEDISCOVERY,
    WAIT_FOR_PAYMENTSERVICESELECTION,
    WAIT_FOR_AUTHORIZATION,
    WAIT_FOR_CHARGEPARAMETERDISCOVERY,
    WAIT_FOR_CABLECHECK,
    WAIT_FOR_PRECHARGE,
    WAIT_FOR_PRECHARGE_POWERDELIVERY,
    WAIT_FOR_CURRENTDEMAND,
    WAIT_FOR_CURRENTDEMAND_POWERDELIVERY,
    WAIT_FOR_WELDINGDETECTION_SESSIONSTOP,
    WAIT_FOR_SESSIONSTOP,
    WAIT_FOR_TERMINATED_SESSION
};

static const char* dinResponse[] = {"Response OK",
                                    "New Session Established",
                                    "Old Session Joined",
                                    "Certificate Expires Soon",
                                    "Response Failed",
                                    "Sequence Error",
                                    "Service ID Invalid",
                                    "Unknown Session",
                                    "Service Selection Invalid",
                                    "Payment Selection Invalid",
                                    "Certificate Expired",
                                    "Signature Error",
                                    "No Certificate Available",
                                    "Cert Chain Error",
                                    "Challenge Invalid",
                                    "Contract Canceled",
                                    "Wrong Charge Parameter",
                                    "Power Delivery Not Applied",
                                    "Tariff Selection Invalid",
                                    "Charging Profile Invalid",
                                    "EVSE Present Voltage To Low",
                                    "Metering Signature Not Valid",
                                    "Wrong Energy Transfer Type"};

/*!
 * \brief The din_state struct
 */
struct din_state {
    const char* description;
    int allowed_requests;
};

static const struct din_state din_states[] = {
    /* [V2G-DC-437]  Expected req msg after supportedAppProtocolRes */
    [WAIT_FOR_SESSIONSETUP] = {"Waiting for SessionSetupReq", 1 << V2G_SESSION_SETUP_MSG},
    /* [V2G-DC-439] Expected req msg after SessionSetupRes */
    [WAIT_FOR_SERVICEDISCOVERY] = {"Waiting for ServiceDiscoveryReq, SessionStopReq",
                                   1 << V2G_SERVICE_DISCOVERY_MSG | 1 << V2G_SESSION_STOP_MSG},
    /* [V2G-DC-441] Expected req msg after ServiceDiscoveryRes */
    [WAIT_FOR_PAYMENTSERVICESELECTION] = {"Waiting for ServicePaymentSelectionReq, SessionStopReq",
                                          1 << V2G_PAYMENT_SERVICE_SELECTION_MSG | 1 << V2G_SESSION_STOP_MSG},
    /* [V2G-DC-444] [V2G-DC-497] Expected req msg after ServicePaymentSelectionRes */
    [WAIT_FOR_AUTHORIZATION] = {"Waiting for ContractAuthenticationReq, SessionStopReq",
                                1 << V2G_AUTHORIZATION_MSG | 1 << V2G_SESSION_STOP_MSG},
    /* [V2G-DC-498], [V2G-DC-495] Expected req msg after ContractAuthenticationRes*/
    [WAIT_FOR_CHARGEPARAMETERDISCOVERY] = {"Waiting for ChargeParameterDiscoveryReq, SessionStopReq",
                                           1 << V2G_CHARGE_PARAMETER_DISCOVERY_MSG | 1 << V2G_SESSION_STOP_MSG},
    /* [V2G-DC-453], [V2G-DC-499] Expected req msg after ChargeParameterDiscoveryRes, CableCheckRes */
    [WAIT_FOR_CABLECHECK] = {"Waiting for CableCheckReq, SessionStopReq",
                             1 << V2G_CABLE_CHECK_MSG | 1 << V2G_SESSION_STOP_MSG},
    /* [V2G-DC-455] Expected req msgs after CableCheckRes */
    [WAIT_FOR_PRECHARGE] = {"Waiting for PreChargeReq, SessionStopReq",
                            1 << V2G_PRE_CHARGE_MSG | 1 << V2G_SESSION_STOP_MSG},
    /* [V2G-DC-458] Expected req msg after PreChargeRes */
    [WAIT_FOR_PRECHARGE_POWERDELIVERY] = {"Waiting for PowerDeliveryReq, PreChargeReq, SessionStopRes",
                                          1 << V2G_POWER_DELIVERY_MSG | 1 << V2G_PRE_CHARGE_MSG |
                                              1 << V2G_SESSION_STOP_MSG},
    /* [V2G-DC-462] Expected req msg after PowerDeliveryRes (Ready to Charge = true) */
    [WAIT_FOR_CURRENTDEMAND] = {"Waiting for CurrentDemandReq", 1 << V2G_CURRENT_DEMAND_MSG},
    /* [V2G-DC-462], [V2G-DC-465] Expected req msg after PowerDeliveryRes (Ready to Charge = true), CurrentDemandRes */
    [WAIT_FOR_CURRENTDEMAND_POWERDELIVERY] = {"Waiting for CurrentDemandReq, PowerDeliveryReq",
                                              1 << V2G_CURRENT_DEMAND_MSG | 1 << V2G_POWER_DELIVERY_MSG},
    /* [V2G-DC-459], [V2G-DC-469] Expected req msg after PowerDeliveryRes, WeldingDetectionRes */
    [WAIT_FOR_WELDINGDETECTION_SESSIONSTOP] = {"Waiting for WeldingDetectionReq, SessionStopReq ",
                                               1 << V2G_WELDING_DETECTION_MSG | 1 << V2G_SESSION_STOP_MSG},
    [WAIT_FOR_SESSIONSTOP] = {"Waiting for SessionStopReq", 1 << V2G_SESSION_STOP_MSG},
    [WAIT_FOR_TERMINATED_SESSION] = {"Terminate session", 0}};

/*!
 * \brief din_handle_request This function handles the incoming request message of a connected EV.
 *  It analyzes the incoming DIN request EXI stream and configures the response EXI stream
 * \param conn This structure provides the EXI streams
 * \return This function returns \c 1 if the connection must be closed immediately,
 *  when this function returns \c 0 the req handle is successful,
 *  when this function returns \c 2 the reply needs to be sent and the connection needs to be closed afterwards
 */
enum v2g_event din_handle_request(v2g_connection* conn);

namespace utils {
enum v2g_event din_validate_response_code(din_responseCodeType* const din_response_code,
                                          struct v2g_connection const* conn);
} // namespace utils

namespace states {

enum v2g_event handle_din_session_setup(struct v2g_connection* conn);
enum v2g_event handle_din_service_discovery(struct v2g_connection* conn);
enum v2g_event handle_din_contract_authentication(struct v2g_connection* conn);

} // namespace states

#endif /* DIN_SERVER_HPP */
