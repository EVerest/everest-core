// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2023 chargebyte GmbH
// Copyright (C) 2023 Contributors to EVerest

#ifndef ISO_SERVER_HPP
#define ISO_SERVER_HPP

#include "v2g.hpp"

struct iso_state {
    const char* description;
    int allowed_requests;
};

enum class iso_ac_state_id {
    WAIT_FOR_SESSIONSETUP = 0,
    WAIT_FOR_SERVICEDISCOVERY,
    WAIT_FOR_SVCDETAIL_PAYMENTSVCSEL,
    WAIT_FOR_PAYMENTDETAILS_CERTINST_CERTUPD,
    WAIT_FOR_PAYMENTDETAILS,
    WAIT_FOR_AUTHORIZATION,
    WAIT_FOR_CHARGEPARAMETERDISCOVERY,
    WAIT_FOR_POWERDELIVERY,
    WAIT_FOR_CHARGINGSTATUS,
    WAIT_FOR_CHARGINGSTATUS_POWERDELIVERY,
    WAIT_FOR_METERINGRECEIPT,
    WAIT_FOR_SESSIONSTOP,
    WAIT_FOR_TERMINATED_SESSION
};

enum class iso_dc_state_id {
    WAIT_FOR_SESSIONSETUP = 0,
    WAIT_FOR_SERVICEDISCOVERY,
    WAIT_FOR_SVCDETAIL_PAYMENTSVCSEL,
    WAIT_FOR_PAYMENTDETAILS_CERTINST_CERTUPD,
    WAIT_FOR_PAYMENTDETAILS,
    WAIT_FOR_AUTHORIZATION,
    WAIT_FOR_CHARGEPARAMETERDISCOVERY,
    WAIT_FOR_CABLECHECK,
    WAIT_FOR_PRECHARGE,
    WAIT_FOR_PRECHARGE_POWERDELIVERY,
    WAIT_FOR_CURRENTDEMAND_POWERDELIVERY,
    WAIT_FOR_CURRENTDEMAND,
    WAIT_FOR_METERINGRECEIPT,
    WAIT_FOR_WELDINGDETECTION_SESSIONSTOP,
    WAIT_FOR_TERMINATED_SESSION
};

static const char* isoResponse[] = {
    "Response OK",
    "New Session Established",
    "Old Session Joined",
    "Certificate Expires Soon",
    "Response FAILED",
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
    "Metering Signature Not Valid",
    "No Charge Service Selected",
    "Wrong Energy Transfer Mode",
    "Contactor Error",
    "Certificate Not Allowed At This EVSE",
    "Certificate Revoked",
};

static const struct iso_state iso_ac_states[] = {
    {"Waiting for SessionSetupReq", 1 << V2G_SESSION_SETUP_MSG},
    /* [V2G-543] Expected req msg after SessionSetupRes */
    {"Waiting for ServiceDiscoveryReq, SessionStopReq", 1 << V2G_SERVICE_DISCOVERY_MSG | 1 << V2G_SESSION_STOP_MSG},
    /* [V2G-545] Expected req msg after ServiceDiscoveryRes */
    {"Waiting for ServiceDetailReq, PaymentServiceSelectionReq, SessionStopReq",
     1 << V2G_SERVICE_DETAIL_MSG | 1 << V2G_PAYMENT_SERVICE_SELECTION_MSG | 1 << V2G_SESSION_STOP_MSG},
    /* [V2G-551] Expected req msg after ServicePaymentSelectionRes */
    {"Waiting for PaymentDetailsReq, CertificateInstallationReq, CertificateUpdateReq, SessionStopReq",
     1 << V2G_PAYMENT_DETAILS_MSG | 1 << V2G_CERTIFICATE_INSTALLATION_MSG | 1 << V2G_CERTIFICATE_UPDATE_MSG |
         1 << V2G_SESSION_STOP_MSG},
    {"Waiting for PaymentDetailsReq, SessionStopReq", 1 << V2G_PAYMENT_DETAILS_MSG | 1 << V2G_SESSION_STOP_MSG},
    {"Waiting for AuthorizationReq, SessionStopReq", 1 << V2G_AUTHORIZATION_MSG | 1 << V2G_SESSION_STOP_MSG},
    {"Waiting for ChargeParameterDiscoveryReq, SessionStopReq",
     1 << V2G_CHARGE_PARAMETER_DISCOVERY_MSG | 1 << V2G_SESSION_STOP_MSG},
    {"Waiting for PowerDeliveryReq, SessionStopReq", 1 << V2G_POWER_DELIVERY_MSG | 1 << V2G_SESSION_STOP_MSG},
    {"Waiting for ChargingStatusReq", 1 << V2G_CHARGING_STATUS_MSG},
    {"Waiting for ChargingStatusReq, PowerDeliveryReq", 1 << V2G_CHARGING_STATUS_MSG | 1 << V2G_POWER_DELIVERY_MSG},
    {"Waiting for MeteringReceiptReq", 1 << V2G_METERING_RECEIPT_MSG},
    {"Waiting for SessionStopReq", 1 << V2G_SESSION_STOP_MSG},
    {"Closing session", 0}};

static const struct iso_state iso_dc_states[] = {
    /* [V2G-541] Expected req msg after SupportedAppProtocolRes */
    {"Waiting for SessionSetupReq", 1 << V2G_SESSION_SETUP_MSG},
    /* [V2G-543] Expected req msg after SessionSetupRes */
    {"Waiting for ServiceDiscoveryReq, SessionStopReq", 1 << V2G_SERVICE_DISCOVERY_MSG | 1 << V2G_SESSION_STOP_MSG},
    /* [V2G-545] Expected req msg after ServiceDiscoveryRes */
    {"Waiting for ServiceDetailReq, ServicePaymentSelectionReq, SessionStopReq",
     1 << V2G_SERVICE_DETAIL_MSG | 1 << V2G_PAYMENT_SERVICE_SELECTION_MSG | 1 << V2G_SESSION_STOP_MSG},
    /* [V2G-551] Expected req msg after ServicePaymentSelectionRes */
    {"Waiting for PaymentDetailsReq, AuthorizationReq, CertificateInstallationReq, CertificateUpdateReq, "
     "SessionStopReq",
     1 << V2G_PAYMENT_DETAILS_MSG | 1 << V2G_AUTHORIZATION_MSG | 1 << V2G_CERTIFICATE_INSTALLATION_MSG |
         1 << V2G_CERTIFICATE_UPDATE_MSG | 1 << V2G_SESSION_STOP_MSG},
    /* [V2G-557], [V2G-554], [V2G-558] Expected req msg after CertificateInstallationRes or CertificateUpdateRes */
    {"Waiting for PaymentDetailsReq, SessionStopReq", 1 << V2G_PAYMENT_DETAILS_MSG | 1 << V2G_SESSION_STOP_MSG},
    /* [V2G-560], [V2G-687] Expected req msg after PaymentDetailsRes, ContractAuthenticationRes */
    {"Waiting for AuthorizationReq, SessionStopReq", 1 << V2G_AUTHORIZATION_MSG | 1 << V2G_SESSION_STOP_MSG},
    /* [V2G-573], [V2G-813],[V2G-688] Expected req msg after AuthorizationRes or PowerDeliveryRes or
       ChargeParameterDiscoveryRes */
    {"Waiting for ChargeParameterDiscoveryReq, SessionStopReq",
     1 << V2G_CHARGE_PARAMETER_DISCOVERY_MSG | 1 << V2G_SESSION_STOP_MSG},
    /* [V2G-582], [V2G-621] Expected req msg after CableCheckRes or ChargeParameterDiscoveryRes */
    {"Waiting for CableCheckReq, SessionStopReq", 1 << V2G_CABLE_CHECK_MSG | 1 << V2G_SESSION_STOP_MSG},
    /* [V2G-584] Expected req msg after CableCheckRes */
    {"Waiting for PreChargeReq, SessionStopReq", 1 << V2G_PRE_CHARGE_MSG | 1 << V2G_SESSION_STOP_MSG},
    /* [V2G-587] Expected req msg after PreChargeRes */
    {"Waiting for PreChargeReq, PowerDeliveryReq, SessionStopReq",
     1 << V2G_PRE_CHARGE_MSG | 1 << V2G_POWER_DELIVERY_MSG | 1 << V2G_SESSION_STOP_MSG},
    /* [V2G-797] Expected req msg after CurrentDemandRes or MeteringReceiptRes*/
    {"Waiting for CurrentDemandReq, PowerDeliveryReq", 1 << V2G_CURRENT_DEMAND_MSG | 1 << V2G_POWER_DELIVERY_MSG},
    /* [V2G-590] Expected req msg after PowerDeliveryRes or CurrentDemandRes or MeteringReceiptRes*/
    {"Waiting for CurrentDemandReq", 1 << V2G_CURRENT_DEMAND_MSG},
    /* [V2G-795] Expected req msg after CurrentDemandRes */
    {"Waiting for MeteringReceiptReq", 1 << V2G_METERING_RECEIPT_MSG},
    /* [V2G-597], [V2G-601] Expected req msg after PowerDeliveryRes or WeldingDetectionRes*/
    {"Waiting for WeldingDetectionReq, SessionStopReq", 1 << V2G_WELDING_DETECTION_MSG | 1 << V2G_SESSION_STOP_MSG},
    {"Closing session", 0}};

/*!
 * \brief iso_handle_request This is the main protocol handler. This function analyzes the received
 *  request msg and configures the next response msg.
 * \param conn \c v2g_connection struct and holds the v2g_connection information
 * \return when this function returns -1 then the connection is aborted without sending the reply,
 *  when this function returns 0 then the reply is sent,
 *  when this function returns 1 then the reply is sent and the connection is closed afterwards,
 *  when this function returns 2 then no reply is sent but the connection is kept open
 */
enum v2g_event iso_handle_request(v2g_connection* conn);

#endif /* ISO_SERVER_HPP */
