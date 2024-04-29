// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2023 chargebyte GmbH
// Copyright (C) 2023 Contributors to EVerest

#include <inttypes.h>
#include <math.h>
#include <mbedtls/base64.h>
#include <mbedtls/error.h>
#include <mbedtls/oid.h> /* To extract the emaid */
#include <mbedtls/sha256.h>
#include <openv2g/EXITypes.h>
#include <openv2g/iso1EXIDatatypes.h>
#include <openv2g/iso1EXIDatatypesEncoder.h>
#include <openv2g/v2gtp.h> //for V2GTP_HEADER_LENGTHs
#include <openv2g/xmldsigEXIDatatypes.h>
#include <openv2g/xmldsigEXIDatatypesEncoder.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iso_server.hpp"
#include "log.hpp"
#include "tools.hpp"
#include "v2g_ctx.hpp"
#include "v2g_server.hpp"

#define MAX_EXI_SIZE                  8192
#define DIGEST_SIZE                   32
#define MQTT_MAX_PAYLOAD_SIZE         268435455
#define V2G_SECC_MSG_CERTINSTALL_TIME 4500
#define MAX_EXI_SIZE                  8192
#define MAX_CERT_SIZE                 800 // [V2G2-010]
#define MAX_EMAID_LEN                 18
#define GEN_CHALLENGE_SIZE            16

const uint16_t SAE_V2H = 28472;
const uint16_t SAE_V2G = 28473;

/*!
 * \brief iso_validate_state This function checks whether the received message is expected and valid at this
 * point in the communication sequence state machine. The current V2G msg type must be set with the current V2G msg
 * state. [V2G2-538]
 * \param state is the current state of the charging session.
 * \param current_v2g_msg is the current handled V2G message.
 * \param is_dc_charging is \c true if it is a DC charging session.
 * \return Returns a iso1responseCode with sequence error if current_v2g_msg is not expected, otherwise OK.
 */
static iso1responseCodeType iso_validate_state(int state, enum V2gMsgTypeId current_v2g_msg, bool is_dc_charging) {

    int allowed_requests =
        (true == is_dc_charging)
            ? iso_dc_states[state].allowed_requests
            : iso_ac_states[state].allowed_requests; // dc_charging is determined in charge_parameter. dc
    return (allowed_requests & (1 << current_v2g_msg)) ? iso1responseCodeType_OK
                                                       : iso1responseCodeType_FAILED_SequenceError;
}

/*!
 * \brief iso_validate_response_code This function checks if an external error has occurred (sequence error, user abort)
 * ... ). \param iso_response_code is a pointer to the current response code. The value will be modified if an external
 *  error has occurred.
 * \param conn the structure with the external error information.
 * \return Returns \c V2G_EVENT_SEND_AND_TERMINATE if the charging must be terminated after sending the response
 * message, returns \c V2G_EVENT_TERMINATE_CONNECTION if charging must be aborted immediately and \c V2G_EVENT_NO_EVENT
 * if no error
 */
static v2g_event iso_validate_response_code(iso1responseCodeType* const v2g_response_code,
                                            struct v2g_connection const* const conn) {
    enum v2g_event next_event = V2G_EVENT_NO_EVENT;
    iso1responseCodeType response_code_tmp;

    if (conn->ctx->is_connection_terminated == true) {
        dlog(DLOG_LEVEL_ERROR, "Connection is terminated. Abort charging");
        return V2G_EVENT_TERMINATE_CONNECTION;
    }

    /* If MQTT user abort or emergency shutdown has occurred */
    if ((conn->ctx->stop_hlc == true) || (conn->ctx->intl_emergency_shutdown == true)) {
        *v2g_response_code = iso1responseCodeType_FAILED;
    }

    /* [V2G-DC-390]: at this point we must check whether the given request is valid at this step;
     * the idea is that we catch this error in each function below to respond with a valid
     * encoded message; note, that the handler functions below must not access v2g_session in
     * error path, since it might not be set, yet!
     */
    response_code_tmp =
        iso_validate_state(conn->ctx->state, conn->ctx->current_v2g_msg, conn->ctx->is_dc_charger); // [V2G2-538]

    *v2g_response_code = (response_code_tmp >= iso1responseCodeType_FAILED) ? response_code_tmp : *v2g_response_code;

    /* [V2G2-460]: check whether the session id matches the expected one of the active session */
    *v2g_response_code = ((conn->ctx->current_v2g_msg != V2G_SESSION_SETUP_MSG) &&
                          (conn->ctx->evse_v2g_data.session_id != conn->ctx->ev_v2g_data.received_session_id))
                             ? iso1responseCodeType_FAILED_UnknownSession
                             : *v2g_response_code;

    if ((conn->ctx->terminate_connection_on_failed_response == true) &&
        (*v2g_response_code >= iso1responseCodeType_FAILED)) {
        next_event = V2G_EVENT_SEND_AND_TERMINATE; // [V2G2-539], [V2G2-034] Send response and terminate tcp-connection
    }

    /* log failed response code message */
    if ((*v2g_response_code >= iso1responseCodeType_FAILED) &&
        (*v2g_response_code <= iso1responseCodeType_FAILED_CertificateRevoked)) {
        dlog(DLOG_LEVEL_ERROR, "Failed response code detected for message \"%s\", error: %s",
             v2g_msg_type[conn->ctx->current_v2g_msg], isoResponse[*v2g_response_code]);
    }

    return next_event;
}

/*!
 * \brief convertIso1ToXmldsigSignedInfoType This function copies V2G iso1SignedInfoType struct into
 * xmldsigSignedInfoType struct type
 * \param xmld_sig_signed_info is the destination struct
 * \param iso1_signed_info is the source struct
 */
static void convertIso1ToXmldsigSignedInfoType(struct xmldsigSignedInfoType* xmld_sig_signed_info,
                                               const struct iso1SignedInfoType* iso1_signed_info) {
    init_xmldsigSignedInfoType(xmld_sig_signed_info);

    for (uint8_t idx = 0; idx < iso1_signed_info->Reference.arrayLen; idx++) {
        const struct iso1ReferenceType* iso1_ref = &iso1_signed_info->Reference.array[idx];
        struct xmldsigReferenceType* xmld_sig_ref = &xmld_sig_signed_info->Reference.array[idx];

        xmld_sig_ref->DigestMethod.Algorithm.charactersLen = iso1_ref->DigestMethod.Algorithm.charactersLen;
        memcpy(xmld_sig_ref->DigestMethod.Algorithm.characters, iso1_ref->DigestMethod.Algorithm.characters,
               iso1_ref->DigestMethod.Algorithm.charactersLen);
        // TODO: Not all elements are copied yet
        xmld_sig_ref->DigestMethod.ANY_isUsed = 0;
        xmld_sig_ref->DigestValue.bytesLen = iso1_ref->DigestValue.bytesLen;
        memcpy(xmld_sig_ref->DigestValue.bytes, iso1_ref->DigestValue.bytes, iso1_ref->DigestValue.bytesLen);

        xmld_sig_ref->Id_isUsed = iso1_ref->Id_isUsed;
        if (0 != iso1_ref->Id_isUsed)
            memcpy(xmld_sig_ref->Id.characters, iso1_ref->Id.characters, iso1_ref->Id.charactersLen);
        xmld_sig_ref->Id.charactersLen = iso1_ref->Id.charactersLen;
        xmld_sig_ref->Transforms_isUsed = iso1_ref->Transforms_isUsed;

        xmld_sig_ref->Transforms.Transform.arrayLen = iso1_ref->Transforms.Transform.arrayLen;
        xmld_sig_ref->Transforms.Transform.array[0].Algorithm.charactersLen =
            iso1_ref->Transforms.Transform.array[0].Algorithm.charactersLen;
        memcpy(xmld_sig_ref->Transforms.Transform.array[0].Algorithm.characters,
               iso1_ref->Transforms.Transform.array[0].Algorithm.characters,
               iso1_ref->Transforms.Transform.array[0].Algorithm.charactersLen);
        xmld_sig_ref->Transforms.Transform.array[0].XPath.arrayLen =
            iso1_ref->Transforms.Transform.array[0].XPath.arrayLen;
        xmld_sig_ref->Transforms.Transform.array[0].ANY_isUsed = 0;
        xmld_sig_ref->Type_isUsed = iso1_ref->Type_isUsed;
        xmld_sig_ref->URI_isUsed = iso1_ref->URI_isUsed;
        xmld_sig_ref->URI.charactersLen = iso1_ref->URI.charactersLen;
        if (0 != iso1_ref->URI_isUsed)
            memcpy(xmld_sig_ref->URI.characters, iso1_ref->URI.characters, iso1_ref->URI.charactersLen);
    }

    xmld_sig_signed_info->Reference.arrayLen = iso1_signed_info->Reference.arrayLen;
    xmld_sig_signed_info->CanonicalizationMethod.ANY_isUsed = 0;
    xmld_sig_signed_info->Id_isUsed = iso1_signed_info->Id_isUsed;
    if (0 != iso1_signed_info->Id_isUsed)
        memcpy(xmld_sig_signed_info->Id.characters, iso1_signed_info->Id.characters,
               iso1_signed_info->Id.charactersLen);
    xmld_sig_signed_info->Id.charactersLen = iso1_signed_info->Id.charactersLen;
    memcpy(xmld_sig_signed_info->CanonicalizationMethod.Algorithm.characters,
           iso1_signed_info->CanonicalizationMethod.Algorithm.characters,
           iso1_signed_info->CanonicalizationMethod.Algorithm.charactersLen);
    xmld_sig_signed_info->CanonicalizationMethod.Algorithm.charactersLen =
        iso1_signed_info->CanonicalizationMethod.Algorithm.charactersLen;

    xmld_sig_signed_info->SignatureMethod.HMACOutputLength_isUsed =
        iso1_signed_info->SignatureMethod.HMACOutputLength_isUsed;
    xmld_sig_signed_info->SignatureMethod.Algorithm.charactersLen =
        iso1_signed_info->SignatureMethod.Algorithm.charactersLen;
    memcpy(xmld_sig_signed_info->SignatureMethod.Algorithm.characters,
           iso1_signed_info->SignatureMethod.Algorithm.characters,
           iso1_signed_info->SignatureMethod.Algorithm.charactersLen);
    xmld_sig_signed_info->SignatureMethod.ANY_isUsed = 0;
}

static bool load_contract_root_cert(mbedtls_x509_crt* contract_root_crt, const char* V2G_file_path,
                                    const char* MO_file_path) {
    int rv = 0;

    if ((rv = mbedtls_x509_crt_parse_file(contract_root_crt, MO_file_path)) != 0) {
        char strerr[256];
        mbedtls_strerror(rv, strerr, sizeof(strerr));
        dlog(DLOG_LEVEL_WARNING, "Unable to parse MO (%s) root certificate. (err: -0x%04x - %s)", MO_file_path, -rv,
             strerr);
        dlog(DLOG_LEVEL_INFO, "Attempting to parse V2G root certificate..");

        if ((rv = mbedtls_x509_crt_parse_file(contract_root_crt, V2G_file_path)) != 0) {
            mbedtls_strerror(rv, strerr, sizeof(strerr));
            dlog(DLOG_LEVEL_ERROR, "Unable to parse V2G (%s) root certificate. (err: -0x%04x - %s)", V2G_file_path, -rv,
                 strerr);
        }
    }

    return (rv != 0) ? false : true;
}

/*!
 * \brief debug_verify_cert Function is from https://github.com/aws/aws-iot-device-sdk-embedded-C/blob
 * /master/platform/linux/mbedtls/network_mbedtls_wrapper.c to debug certificate verification
 * \param data
 * \param crt
 * \param depth
 * \param flags
 * \return
 */
static int debug_verify_cert(void* data, mbedtls_x509_crt* crt, int depth, uint32_t* flags) {
    char buf[1024];
    ((void)data);

    dlog(DLOG_LEVEL_INFO, "\nVerify requested for (Depth %d):\n", depth);
    mbedtls_x509_crt_info(buf, sizeof(buf) - 1, "", crt);
    dlog(DLOG_LEVEL_INFO, "%s", buf);

    if ((*flags) == 0)
        dlog(DLOG_LEVEL_INFO, "  This certificate has no flags\n");
    else {
        mbedtls_x509_crt_verify_info(buf, sizeof(buf), "  ! ", *flags);
        dlog(DLOG_LEVEL_INFO, "%s\n", buf);
    }

    return (0);
}

/*!
 * \brief printMbedVerifyErrorCode This functions prints the mbedTls specific error code.
 * \param AErr is the return value of the mbed verify function
 * \param AFlags includes the flags of the verification.
 */
static void printMbedVerifyErrorCode(int AErr, uint32_t AFlags) {
    dlog(DLOG_LEVEL_ERROR, "Failed to verify certificate (err: 0x%08x flags: 0x%08x)", AErr, AFlags);
    if (AErr == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
        if (AFlags & MBEDTLS_X509_BADCERT_EXPIRED)
            dlog(DLOG_LEVEL_ERROR, "CERT_EXPIRED");
        else if (AFlags & MBEDTLS_X509_BADCERT_REVOKED)
            dlog(DLOG_LEVEL_ERROR, "CERT_REVOKED");
        else if (AFlags & MBEDTLS_X509_BADCERT_CN_MISMATCH)
            dlog(DLOG_LEVEL_ERROR, "CN_MISMATCH");
        else if (AFlags & MBEDTLS_X509_BADCERT_NOT_TRUSTED)
            dlog(DLOG_LEVEL_ERROR, "CERT_NOT_TRUSTED");
        else if (AFlags & MBEDTLS_X509_BADCRL_NOT_TRUSTED)
            dlog(DLOG_LEVEL_ERROR, "CRL_NOT_TRUSTED");
        else if (AFlags & MBEDTLS_X509_BADCRL_EXPIRED)
            dlog(DLOG_LEVEL_ERROR, "CRL_EXPIRED");
        else if (AFlags & MBEDTLS_X509_BADCERT_MISSING)
            dlog(DLOG_LEVEL_ERROR, "CERT_MISSING");
        else if (AFlags & MBEDTLS_X509_BADCERT_SKIP_VERIFY)
            dlog(DLOG_LEVEL_ERROR, "SKIP_VERIFY");
        else if (AFlags & MBEDTLS_X509_BADCERT_OTHER)
            dlog(DLOG_LEVEL_ERROR, "CERT_OTHER");
        else if (AFlags & MBEDTLS_X509_BADCERT_FUTURE)
            dlog(DLOG_LEVEL_ERROR, "CERT_FUTURE");
        else if (AFlags & MBEDTLS_X509_BADCRL_FUTURE)
            dlog(DLOG_LEVEL_ERROR, "CRL_FUTURE");
        else if (AFlags & MBEDTLS_X509_BADCERT_KEY_USAGE)
            dlog(DLOG_LEVEL_ERROR, "KEY_USAGE");
        else if (AFlags & MBEDTLS_X509_BADCERT_EXT_KEY_USAGE)
            dlog(DLOG_LEVEL_ERROR, "EXT_KEY_USAGE");
        else if (AFlags & MBEDTLS_X509_BADCERT_NS_CERT_TYPE)
            dlog(DLOG_LEVEL_ERROR, "NS_CERT_TYPE");
        else if (AFlags & MBEDTLS_X509_BADCERT_BAD_MD)
            dlog(DLOG_LEVEL_ERROR, "BAD_MD");
        else if (AFlags & MBEDTLS_X509_BADCERT_BAD_PK)
            dlog(DLOG_LEVEL_ERROR, "BAD_PK");
        else if (AFlags & MBEDTLS_X509_BADCERT_BAD_KEY)
            dlog(DLOG_LEVEL_ERROR, "BAD_KEY");
        else if (AFlags & MBEDTLS_X509_BADCRL_BAD_MD)
            dlog(DLOG_LEVEL_ERROR, "CRL_BAD_MD");
        else if (AFlags & MBEDTLS_X509_BADCRL_BAD_PK)
            dlog(DLOG_LEVEL_ERROR, "CRL_BAD_PK");
        else if (AFlags & MBEDTLS_X509_BADCRL_BAD_KEY)
            dlog(DLOG_LEVEL_ERROR, "CRL_BAD_KEY");
    }
}

/*!
 * \brief check_iso1_signature This function validates the ISO signature
 * \param iso1_signature is the signature of the ISO EXI fragment
 * \param public_key is the public key to validate the signature against the ISO EXI fragment
 * \param iso1_exi_fragment iso1_exi_fragment is the ISO EXI fragment
 */
static bool check_iso1_signature(const struct iso1SignatureType* iso1_signature, mbedtls_ecdsa_context* public_key,
                                 struct iso1EXIFragment* iso1_exi_fragment) {
    /** Digest check **/
    int err = 0;
    const struct iso1SignatureType* sig = iso1_signature;
    unsigned char buf[MAX_EXI_SIZE];
    size_t buffer_pos = 0;
    const struct iso1ReferenceType* req_ref = &sig->SignedInfo.Reference.array[0];
    bitstream_t stream = {MAX_EXI_SIZE, buf, &buffer_pos, 0, 8 /* Set to 8 for send and 0 for recv */};
    uint8_t digest[DIGEST_SIZE];
    err = encode_iso1ExiFragment(&stream, iso1_exi_fragment);
    if (err != 0) {
        dlog(DLOG_LEVEL_ERROR, "Unable to encode fragment, error code = %d", err);
        return false;
    }
    mbedtls_sha256(buf, buffer_pos, digest, 0);

    if (req_ref->DigestValue.bytesLen != DIGEST_SIZE) {
        dlog(DLOG_LEVEL_ERROR, "Invalid digest length %u in signature", req_ref->DigestValue.bytesLen);
        return false;
    }

    if (memcmp(req_ref->DigestValue.bytes, digest, DIGEST_SIZE) != 0) {
        dlog(DLOG_LEVEL_ERROR, "Invalid digest in signature");
        return false;
    }

    /** Validate signature **/
    struct xmldsigEXIFragment sig_fragment;
    init_xmldsigEXIFragment(&sig_fragment);
    sig_fragment.SignedInfo_isUsed = 1;
    convertIso1ToXmldsigSignedInfoType(&sig_fragment.SignedInfo, &sig->SignedInfo);

    buffer_pos = 0;
    err = encode_xmldsigExiFragment(&stream, &sig_fragment);

    if (err != 0) {
        dlog(DLOG_LEVEL_ERROR, "Unable to encode XML signature fragment, error code = %d", err);
        return false;
    }

    /* Hash the signature */
    mbedtls_sha256(buf, buffer_pos, digest, 0);

    /* Validate the ecdsa signature using the public key */
    if (sig->SignatureValue.CONTENT.bytesLen == 0) {
        dlog(DLOG_LEVEL_ERROR, "Signature len is invalid (%i)", sig->SignatureValue.CONTENT.bytesLen);
        return false;
    }

    /* Init mbedtls parameter */
    mbedtls_ecp_group ecp_group;
    mbedtls_ecp_group_init(&ecp_group);

    mbedtls_mpi mpi_r;
    mbedtls_mpi_init(&mpi_r);
    mbedtls_mpi mpi_s;
    mbedtls_mpi_init(&mpi_s);

    mbedtls_mpi_read_binary(&mpi_r, (const unsigned char*)&sig->SignatureValue.CONTENT.bytes[0],
                            sig->SignatureValue.CONTENT.bytesLen / 2);
    mbedtls_mpi_read_binary(
        &mpi_s, (const unsigned char*)&sig->SignatureValue.CONTENT.bytes[sig->SignatureValue.CONTENT.bytesLen / 2],
        sig->SignatureValue.CONTENT.bytesLen / 2);

    err = mbedtls_ecp_group_load(&ecp_group, MBEDTLS_ECP_DP_SECP256R1);

    if (err == 0) {
        err = mbedtls_ecdsa_verify(&ecp_group, static_cast<const unsigned char*>(digest), 32, &public_key->Q, &mpi_r,
                                   &mpi_s);
    }

    mbedtls_ecp_group_free(&ecp_group);
    mbedtls_mpi_free(&mpi_r);
    mbedtls_mpi_free(&mpi_s);

    if (err != 0) {
        char error_buf[100];
        mbedtls_strerror(err, error_buf, sizeof(error_buf));
        dlog(DLOG_LEVEL_ERROR, "Invalid signature, error code = -0x%08x, %s", err, error_buf);
        return false;
    }

    return true;
}

/*!
 * \brief populate_ac_evse_status This function configures the evse_status struct
 * \param ctx is the V2G context
 * \param evse_status is the destination struct
 */
static void populate_ac_evse_status(struct v2g_context* ctx, struct iso1AC_EVSEStatusType* evse_status) {
    evse_status->EVSENotification = (iso1EVSENotificationType)ctx->evse_v2g_data.evse_notification;
    evse_status->NotificationMaxDelay = ctx->evse_v2g_data.notification_max_delay;
    evse_status->RCD = ctx->evse_v2g_data.rcd;
}

/*!
 * \brief check_iso1_charging_profile_values This function checks if EV charging profile values are within permissible
 * ranges \param req is the PowerDeliveryReq \param res is the PowerDeliveryRes \param conn holds the structure with the
 * V2G msg pair \param sa_schedule_tuple_idx is the index of SA schedule tuple
 */
static void check_iso1_charging_profile_values(iso1PowerDeliveryReqType* req, iso1PowerDeliveryResType* res,
                                               v2g_connection* conn, uint8_t sa_schedule_tuple_idx) {
    if (req->ChargingProfile_isUsed == (unsigned int)1) {

        const struct iso1PMaxScheduleType* evse_p_max_schedule =
            &conn->ctx->evse_v2g_data.evse_sa_schedule_list.SAScheduleTuple.array[sa_schedule_tuple_idx].PMaxSchedule;

        uint32_t ev_time_sum = 0;                     // Summed EV relative time interval
        uint32_t evse_time_sum = 0;                   // Summed EVSE relative time interval
        uint8_t evse_idx = 0;                         // Actual PMaxScheduleEntry index
        bool ev_time_is_within_profile_entry = false; /* Is true if the summed EV relative time interval
                                                         is within the actual EVSE time interval */

        /* Check if the EV ChargingProfileEntryStart time and PMax value fits with the provided EVSE PMaxScheduleEntry
         * list. [V2G2-293] */
        for (uint8_t ev_idx = 0;
             ev_idx < req->ChargingProfile.ProfileEntry.arrayLen && (res->ResponseCode == iso1responseCodeType_OK);
             ev_idx++) {

            ev_time_sum += req->ChargingProfile.ProfileEntry.array[ev_idx].ChargingProfileEntryStart;

            while (evse_idx < evse_p_max_schedule->PMaxScheduleEntry.arrayLen &&
                   (ev_time_is_within_profile_entry == false)) {

                /* Check if EV ChargingProfileEntryStart value is within one EVSE schedule entry.
                 * The last element must be checked separately, because of the duration value */

                /* If we found an entry which fits in the EVSE time schedule, check if the next EV time slot fits as
                 * well Otherwise check if the next time interval fits in the EVSE time schedule */
                evse_time_sum += evse_p_max_schedule->PMaxScheduleEntry.array[evse_idx].RelativeTimeInterval.start;

                /* Check the time intervals, in the last schedule element the duration value must be considered */
                if (evse_idx < (evse_p_max_schedule->PMaxScheduleEntry.arrayLen - 1)) {
                    ev_time_is_within_profile_entry =
                        (ev_time_sum >= evse_time_sum) &&
                        (ev_time_sum <
                         (evse_time_sum +
                          evse_p_max_schedule->PMaxScheduleEntry.array[evse_idx + 1].RelativeTimeInterval.start));
                } else {
                    ev_time_is_within_profile_entry =
                        (ev_time_sum >= evse_time_sum) &&
                        (ev_time_sum <=
                         (evse_time_sum +
                          evse_p_max_schedule->PMaxScheduleEntry.array[evse_idx].RelativeTimeInterval.duration_isUsed *
                              evse_p_max_schedule->PMaxScheduleEntry.array[evse_idx].RelativeTimeInterval.duration));
                }

                if (ev_time_is_within_profile_entry == true) {
                    /* Check if ev ChargingProfileEntryMaxPower element is equal to or smaller than the limits in
                     * respective elements of the PMaxScheduleType */
                    if ((req->ChargingProfile.ProfileEntry.array[ev_idx].ChargingProfileEntryMaxPower.Value *
                         pow(10,
                             req->ChargingProfile.ProfileEntry.array[ev_idx].ChargingProfileEntryMaxPower.Multiplier)) >
                        (evse_p_max_schedule->PMaxScheduleEntry.array[evse_idx].PMax.Value *
                         pow(10, evse_p_max_schedule->PMaxScheduleEntry.array[evse_idx].PMax.Multiplier))) {
                        // res->ResponseCode = iso1responseCodeType_FAILED_ChargingProfileInvalid; // [V2G2-224]
                        // [V2G2-225] [V2G2-478]
                        //  setting response code is commented because some EVs do not support schedules correctly
                        dlog(DLOG_LEVEL_WARNING,
                             "EV's charging profile is invalid (ChargingProfileEntryMaxPower too high)!");
                        break;
                    }
                }
                /* If the last EVSE element is reached and ChargingProfileEntryStart time doesn't fit */
                else if (evse_idx == (evse_p_max_schedule->PMaxScheduleEntry.arrayLen - 1)) {
                    // res->ResponseCode = iso1responseCodeType_FAILED_ChargingProfileInvalid; // EV charing profile
                    // time exceeds EVSE provided schedule
                    //  setting response code is commented because some EVs do not support schedules correctly
                    dlog(DLOG_LEVEL_WARNING,
                         "EV's charging profile is invalid (EV charging profile time exceeds provided schedule)!");
                } else {
                    /* Now we checked if the current EV interval fits within the EVSE interval, but it fails.
                     * Next step is to check the EVSE interval until we reached the last EVSE interval */
                    evse_idx++;
                }
            }
        }
    }
}

static void publish_DC_EVStatusType(struct v2g_context* ctx, const struct iso1DC_EVStatusType& iso1_ev_status) {
    if ((ctx->ev_v2g_data.iso1_dc_ev_status.EVErrorCode != iso1_ev_status.EVErrorCode) ||
        (ctx->ev_v2g_data.iso1_dc_ev_status.EVReady != iso1_ev_status.EVReady) ||
        (ctx->ev_v2g_data.iso1_dc_ev_status.EVRESSSOC != iso1_ev_status.EVRESSSOC)) {
        ctx->ev_v2g_data.iso1_dc_ev_status.EVErrorCode = iso1_ev_status.EVErrorCode;
        ctx->ev_v2g_data.iso1_dc_ev_status.EVReady = iso1_ev_status.EVReady;
        ctx->ev_v2g_data.iso1_dc_ev_status.EVRESSSOC = iso1_ev_status.EVRESSSOC;

        types::iso15118_charger::DC_EVStatusType ev_status;
        ev_status.DC_EVErrorCode = static_cast<types::iso15118_charger::DC_EVErrorCode>(iso1_ev_status.EVErrorCode);
        ev_status.DC_EVReady = iso1_ev_status.EVReady;
        ev_status.DC_EVRESSSOC = static_cast<float>(iso1_ev_status.EVRESSSOC);
        ctx->p_charger->publish_DC_EVStatus(ev_status);
    }
}

static bool getSubjectData(const mbedtls_x509_name* ASubject, const char* AAttrName, const mbedtls_asn1_buf** AVal) {
    const char* attrName = NULL;

    while (NULL != ASubject) {
        if ((0 == mbedtls_oid_get_attr_short_name(&ASubject->oid, &attrName)) && (0 == strcmp(attrName, AAttrName))) {

            *AVal = &ASubject->val;
            return true;
        } else {
            ASubject = ASubject->next;
        }
    }
    *AVal = NULL;
    return false;
}

/*!
 * \brief getEmaidFromContractCert This function extracts the emaid from an subject of a contract certificate.
 * \param ASubject is the subject of the contract certificate.
 * \param AEmaid is the buffer for the extracted emaid. The extracted string is null terminated.
 * \param AEmaidLen is length of the AEmaid buffer.
 * \return Returns the length of the extracted emaid.
 */
static size_t getEmaidFromContractCert(const mbedtls_x509_name* ASubject, char* AEmaid, uint8_t AEmaidLen) {

    const mbedtls_asn1_buf* val = NULL;
    size_t certEmaidLen = 0;
    char certEmaid[MAX_EMAID_LEN + 1];

    if (true == getSubjectData(ASubject, "CN", &val)) {
        /* Check the emaid length within the certificate */
        certEmaidLen = MAX_EMAID_LEN < val->len ? 0 : val->len;
        strncpy(certEmaid, reinterpret_cast<const char*>(val->p), certEmaidLen);
        certEmaid[certEmaidLen] = '\0';

        /* Filter '-' character */
        certEmaidLen = 0;
        for (uint8_t idx = 0; certEmaid[idx] != '\0'; idx++) {
            if ('-' != certEmaid[idx]) {
                certEmaid[certEmaidLen++] = certEmaid[idx];
            }
        }
        certEmaid[certEmaidLen] = '\0';

        if (certEmaidLen > (AEmaidLen - 1)) {
            dlog(DLOG_LEVEL_WARNING, "emaid buffer is too small (%i received, expected %i)", certEmaidLen,
                 AEmaidLen - 1);
            return 0;
        } else {
            strcpy(AEmaid, certEmaid);
        }
    } else {
        dlog(DLOG_LEVEL_WARNING, "No CN found in subject of the contract certificate");
    }

    return certEmaidLen;
}

//=============================================
//             Publishing request msg
//=============================================

/*!
 * \brief publish_iso_service_discovery_req This function publishes the iso_service_discovery_req message to the MQTT
 * interface. \param iso1ServiceDiscoveryReqType is the request message.
 */
static void
publish_iso_service_discovery_req(struct iso1ServiceDiscoveryReqType const* const v2g_service_discovery_req) {
    // V2G values that can be published: ServiceCategory, ServiceScope
}

/*!
 * \brief publish_iso_service_detail_req This function publishes the iso_service_detail_req message to the MQTT
 * interface. \param v2g_service_detail_req is the request message.
 */
static void publish_iso_service_detail_req(struct iso1ServiceDetailReqType const* const v2g_service_detail_req) {
    // V2G values that can be published: ServiceID
}

/*!
 * \brief publish_iso_payment_service_selection_req This function publishes the iso_payment_service_selection_req
 * message to the MQTT interface.
 * \param v2g_payment_service_selection_req is the request message.
 */
static void publish_iso_payment_service_selection_req(
    struct iso1PaymentServiceSelectionReqType const* const v2g_payment_service_selection_req) {
    // V2G values that can be published: SelectedPaymentOption, SelectedServiceList
}

/*!
 * \brief publish_iso_authorization_req This function publishes the publish_iso_authorization_req message to the MQTT
 * interface. \param v2g_authorization_req is the request message.
 */
static void publish_iso_authorization_req(struct iso1AuthorizationReqType const* const v2g_authorization_req) {
    // V2G values that can be published: Id, Id_isUsed, GenChallenge, GenChallenge_isUsed
}

/*!
 * \brief publish_iso_charge_parameter_discovery_req This function publishes the charge_parameter_discovery_req message
 * to the MQTT interface. \param ctx is the V2G context. \param v2g_charge_parameter_discovery_req is the request
 * message.
 */
static void publish_iso_charge_parameter_discovery_req(
    struct v2g_context* ctx,
    struct iso1ChargeParameterDiscoveryReqType const* const v2g_charge_parameter_discovery_req) {
    // V2G values that can be published: DC_EVChargeParameter, MaxEntriesSAScheduleTuple
    ctx->p_charger->publish_RequestedEnergyTransferMode(static_cast<types::iso15118_charger::EnergyTransferMode>(
        v2g_charge_parameter_discovery_req->RequestedEnergyTransferMode));
    if (v2g_charge_parameter_discovery_req->AC_EVChargeParameter_isUsed == (unsigned int)1) {
        if (v2g_charge_parameter_discovery_req->AC_EVChargeParameter.DepartureTime_isUsed == (unsigned int)1) {
            const char* format = "%Y-%m-%dT%H:%M:%SZ";
            char buffer[100];
            std::time_t time_now_in_sec = time(NULL);
            std::time_t departure_time =
                time_now_in_sec + v2g_charge_parameter_discovery_req->AC_EVChargeParameter.DepartureTime;
            std::strftime(buffer, sizeof(buffer), format, std::gmtime(&departure_time));
            ctx->p_charger->publish_DepartureTime(buffer);
        }
        ctx->p_charger->publish_AC_EAmount(
            calc_physical_value(v2g_charge_parameter_discovery_req->AC_EVChargeParameter.EAmount.Value,
                                v2g_charge_parameter_discovery_req->AC_EVChargeParameter.EAmount.Multiplier));
        ctx->p_charger->publish_AC_EVMaxVoltage(
            calc_physical_value(v2g_charge_parameter_discovery_req->AC_EVChargeParameter.EVMaxVoltage.Value,
                                v2g_charge_parameter_discovery_req->AC_EVChargeParameter.EVMaxVoltage.Multiplier));
        ctx->p_charger->publish_AC_EVMaxCurrent(
            calc_physical_value(v2g_charge_parameter_discovery_req->AC_EVChargeParameter.EVMaxCurrent.Value,
                                v2g_charge_parameter_discovery_req->AC_EVChargeParameter.EVMaxCurrent.Multiplier));
        ctx->p_charger->publish_AC_EVMinCurrent(
            calc_physical_value(v2g_charge_parameter_discovery_req->AC_EVChargeParameter.EVMinCurrent.Value,
                                v2g_charge_parameter_discovery_req->AC_EVChargeParameter.EVMinCurrent.Multiplier));
    } else if (v2g_charge_parameter_discovery_req->DC_EVChargeParameter_isUsed == (unsigned int)1) {
        if (v2g_charge_parameter_discovery_req->DC_EVChargeParameter.DepartureTime_isUsed == (unsigned int)1) {
            const char* format = "%Y-%m-%dT%H:%M:%SZ";
            char buffer[100];
            std::time_t time_now_in_sec = time(NULL);
            std::time_t departure_time =
                time_now_in_sec + v2g_charge_parameter_discovery_req->DC_EVChargeParameter.DepartureTime;
            std::strftime(buffer, sizeof(buffer), format, std::gmtime(&departure_time));
            ctx->p_charger->publish_DepartureTime(buffer);

            if (v2g_charge_parameter_discovery_req->DC_EVChargeParameter.EVEnergyCapacity_isUsed == (unsigned int)1) {
                ctx->p_charger->publish_DC_EVEnergyCapacity(calc_physical_value(
                    v2g_charge_parameter_discovery_req->DC_EVChargeParameter.EVEnergyCapacity.Value,
                    v2g_charge_parameter_discovery_req->DC_EVChargeParameter.EVEnergyCapacity.Multiplier));
            }
            if (v2g_charge_parameter_discovery_req->DC_EVChargeParameter.EVEnergyRequest_isUsed == (unsigned int)1) {
                ctx->p_charger->publish_DC_EVEnergyRequest(calc_physical_value(
                    v2g_charge_parameter_discovery_req->DC_EVChargeParameter.EVEnergyRequest.Value,
                    v2g_charge_parameter_discovery_req->DC_EVChargeParameter.EVEnergyRequest.Multiplier));
            }
            if (v2g_charge_parameter_discovery_req->DC_EVChargeParameter.FullSOC_isUsed == (unsigned int)1) {
                ctx->p_charger->publish_DC_FullSOC(v2g_charge_parameter_discovery_req->DC_EVChargeParameter.FullSOC);
            }
            if (v2g_charge_parameter_discovery_req->DC_EVChargeParameter.BulkSOC_isUsed == (unsigned int)1) {
                ctx->p_charger->publish_DC_BulkSOC(v2g_charge_parameter_discovery_req->DC_EVChargeParameter.BulkSOC);
            }
            float evMaximumCurrentLimit = calc_physical_value(
                v2g_charge_parameter_discovery_req->DC_EVChargeParameter.EVMaximumCurrentLimit.Value,
                v2g_charge_parameter_discovery_req->DC_EVChargeParameter.EVMaximumCurrentLimit.Multiplier);
            float evMaximumPowerLimit = calc_physical_value(
                v2g_charge_parameter_discovery_req->DC_EVChargeParameter.EVMaximumPowerLimit.Value,
                v2g_charge_parameter_discovery_req->DC_EVChargeParameter.EVMaximumPowerLimit.Multiplier);
            float evMaximumVoltageLimit = calc_physical_value(
                v2g_charge_parameter_discovery_req->DC_EVChargeParameter.EVMaximumVoltageLimit.Value,
                v2g_charge_parameter_discovery_req->DC_EVChargeParameter.EVMaximumVoltageLimit.Multiplier);
            publish_DC_EVMaximumLimits(
                ctx, evMaximumCurrentLimit, (unsigned int)1, evMaximumPowerLimit,
                v2g_charge_parameter_discovery_req->DC_EVChargeParameter.EVMaximumPowerLimit_isUsed,
                evMaximumVoltageLimit, (unsigned int)1);
            publish_DC_EVStatusType(ctx, v2g_charge_parameter_discovery_req->DC_EVChargeParameter.DC_EVStatus);
        }
    }
}

/*!
 * \brief publish_iso_pre_charge_req This function publishes the iso_pre_charge_req message to the MQTT interface.
 * \param ctx is the V2G context.
 * \param v2g_precharge_req is the request message.
 */
static void publish_iso_pre_charge_req(struct v2g_context* ctx,
                                       struct iso1PreChargeReqType const* const v2g_precharge_req) {
    publish_DC_EVTargetVoltageCurrent(
        ctx,
        calc_physical_value(v2g_precharge_req->EVTargetVoltage.Value, v2g_precharge_req->EVTargetVoltage.Multiplier),
        calc_physical_value(v2g_precharge_req->EVTargetCurrent.Value, v2g_precharge_req->EVTargetCurrent.Multiplier));
    publish_DC_EVStatusType(ctx, v2g_precharge_req->DC_EVStatus);
}

/*!
 * \brief publish_iso_power_delivery_req This function publishes the iso_power_delivery_req message to the MQTT
 * interface. \param ctx is the V2G context. \param v2g_power_delivery_req is the request message.
 */
static void publish_iso_power_delivery_req(struct v2g_context* ctx,
                                           struct iso1PowerDeliveryReqType const* const v2g_power_delivery_req) {
    // V2G values that can be published: ChargeProgress, SAScheduleTupleID
    if (v2g_power_delivery_req->DC_EVPowerDeliveryParameter_isUsed == (unsigned int)1) {
        ctx->p_charger->publish_DC_ChargingComplete(
            v2g_power_delivery_req->DC_EVPowerDeliveryParameter.ChargingComplete);
        if (v2g_power_delivery_req->DC_EVPowerDeliveryParameter.BulkChargingComplete_isUsed == (unsigned int)1) {
            ctx->p_charger->publish_DC_BulkChargingComplete(
                v2g_power_delivery_req->DC_EVPowerDeliveryParameter.BulkChargingComplete);
        }
        publish_DC_EVStatusType(ctx, v2g_power_delivery_req->DC_EVPowerDeliveryParameter.DC_EVStatus);
    }
}

/*!
 * \brief publish_iso_current_demand_req This function publishes the iso_current_demand_req message to the MQTT
 * interface. \param ctx is the V2G context \param v2g_current_demand_req is the request message.
 */
static void publish_iso_current_demand_req(struct v2g_context* ctx,
                                           struct iso1CurrentDemandReqType const* const v2g_current_demand_req) {
    if ((v2g_current_demand_req->BulkChargingComplete_isUsed == (unsigned int)1) &&
        (ctx->ev_v2g_data.bulk_charging_complete != v2g_current_demand_req->BulkChargingComplete)) {
        ctx->p_charger->publish_DC_BulkChargingComplete(v2g_current_demand_req->BulkChargingComplete);
        ctx->ev_v2g_data.bulk_charging_complete = v2g_current_demand_req->BulkChargingComplete;
    }
    if (ctx->ev_v2g_data.charging_complete != v2g_current_demand_req->ChargingComplete) {
        ctx->p_charger->publish_DC_ChargingComplete(v2g_current_demand_req->ChargingComplete);
        ctx->ev_v2g_data.charging_complete = v2g_current_demand_req->ChargingComplete;
    }

    publish_DC_EVStatusType(ctx, v2g_current_demand_req->DC_EVStatus);

    publish_DC_EVTargetVoltageCurrent(ctx,
                                      calc_physical_value(v2g_current_demand_req->EVTargetVoltage.Value,
                                                          v2g_current_demand_req->EVTargetVoltage.Multiplier),
                                      calc_physical_value(v2g_current_demand_req->EVTargetCurrent.Value,
                                                          v2g_current_demand_req->EVTargetCurrent.Multiplier));

    float evMaximumCurrentLimit = calc_physical_value(v2g_current_demand_req->EVMaximumCurrentLimit.Value,
                                                      v2g_current_demand_req->EVMaximumCurrentLimit.Multiplier);
    float evMaximumPowerLimit = calc_physical_value(v2g_current_demand_req->EVMaximumPowerLimit.Value,
                                                    v2g_current_demand_req->EVMaximumPowerLimit.Multiplier);
    float evMaximumVoltageLimit = calc_physical_value(v2g_current_demand_req->EVMaximumVoltageLimit.Value,
                                                      v2g_current_demand_req->EVMaximumVoltageLimit.Multiplier);

    publish_DC_EVMaximumLimits(ctx, evMaximumCurrentLimit, v2g_current_demand_req->EVMaximumCurrentLimit_isUsed,
                               evMaximumPowerLimit, v2g_current_demand_req->EVMaximumPowerLimit_isUsed,
                               evMaximumVoltageLimit, v2g_current_demand_req->EVMaximumVoltageLimit_isUsed);

    float v2g_dc_ev_remaining_time_to_full_soc =
        calc_physical_value(v2g_current_demand_req->RemainingTimeToFullSoC.Value,
                            v2g_current_demand_req->RemainingTimeToFullSoC.Multiplier);
    float v2g_dc_ev_remaining_time_to_bulk_soc =
        calc_physical_value(v2g_current_demand_req->RemainingTimeToBulkSoC.Value,
                            v2g_current_demand_req->RemainingTimeToBulkSoC.Multiplier);
    publish_DC_EVRemainingTime(
        ctx, v2g_dc_ev_remaining_time_to_full_soc, v2g_current_demand_req->RemainingTimeToFullSoC_isUsed,
        v2g_dc_ev_remaining_time_to_bulk_soc, v2g_current_demand_req->RemainingTimeToBulkSoC_isUsed);
}
/*!
 * \brief publish_iso_metering_receipt_req This function publishes the iso_metering_receipt_req message to the MQTT
 * interface. \param v2g_metering_receipt_req is the request message.
 */
static void publish_iso_metering_receipt_req(struct iso1MeteringReceiptReqType const* const v2g_metering_receipt_req) {
    // TODO: publish PnC only
}

/*!
 * \brief publish_iso_welding_detection_req This function publishes the iso_welding_detection_req message to the MQTT
 * interface. \param p_charger to publish MQTT topics. \param v2g_welding_detection_req is the request message.
 */
static void
publish_iso_welding_detection_req(struct v2g_context* ctx,
                                  struct iso1WeldingDetectionReqType const* const v2g_welding_detection_req) {
    // TODO: V2G values that can be published: EVErrorCode, EVReady, EVRESSSOC
    publish_DC_EVStatusType(ctx, v2g_welding_detection_req->DC_EVStatus);
}

/*!
 * \brief publish_iso_certificate_installation_exi_req This function publishes the iso_certificate_update_req message to
 * the MQTT interface. \param AExiBuffer is the exi msg where the V2G EXI msg is stored. \param AExiBufferSize is the
 * size of the V2G msg. \return Returns \c true if it was successful, otherwise \c false.
 */
static bool publish_iso_certificate_installation_exi_req(struct v2g_context* ctx, uint8_t* AExiBuffer,
                                                         size_t AExiBufferSize) {
    // PnC only

    bool rv = true;
    unsigned char* base64Buffer = NULL;
    size_t olen;
    types::iso15118_charger::Request_Exi_Stream_Schema certificate_request;

    /* Parse contract leaf certificate */
    mbedtls_base64_encode(NULL, 0, &olen, static_cast<unsigned char*>(AExiBuffer), AExiBufferSize);

    if (MQTT_MAX_PAYLOAD_SIZE < olen) {
        rv = false;
        dlog(DLOG_LEVEL_ERROR, "Mqtt payload size exceeded!");
        goto exit;
    }
    base64Buffer = static_cast<unsigned char*>(malloc(olen));

    if ((NULL == base64Buffer) ||
        (mbedtls_base64_encode(base64Buffer, olen, &olen, static_cast<unsigned char*>(AExiBuffer), AExiBufferSize) !=
         0)) {
        rv = false;
        dlog(DLOG_LEVEL_ERROR, "Unable to encode contract leaf certificate");
        goto exit;
    }

    certificate_request.exiRequest = std::string(reinterpret_cast<const char*>(base64Buffer), olen);
    certificate_request.iso15118SchemaVersion = ISO_15118_2013_MSG_DEF;
    certificate_request.certificateAction = types::iso15118_charger::CertificateActionEnum::Install;
    ctx->p_charger->publish_Certificate_Request(certificate_request);

exit:
    if (base64Buffer != NULL)
        free(base64Buffer);

    return rv;
}

//=============================================
//             Request Handling
//=============================================

/*!
 * \brief handle_iso_session_setup This function handles the iso_session_setup msg pair. It analyzes the request msg and
 * fills the response msg. \param conn holds the structure with the V2G msg pair. \return Returns the next V2G-event.
 */
static enum v2g_event handle_iso_session_setup(struct v2g_connection* conn) {
    struct iso1SessionSetupReqType* req = &conn->exi_in.iso1EXIDocument->V2G_Message.Body.SessionSetupReq;
    struct iso1SessionSetupResType* res = &conn->exi_out.iso1EXIDocument->V2G_Message.Body.SessionSetupRes;
    char buffer[iso1SessionSetupReqType_EVCCID_BYTES_SIZE * 3 - 1 + 1]; /* format: (%02x:) * n - (1x ':') + (1x NULL) */
    int i;
    enum v2g_event next_event = V2G_EVENT_NO_EVENT;

    /* format EVCC ID */
    for (i = 0; i < req->EVCCID.bytesLen; i++) {
        sprintf(&buffer[i * 3], "%02" PRIX8 ":", req->EVCCID.bytes[i]);
    }
    if (i)
        buffer[i * 3 - 1] = '\0';
    else
        buffer[0] = '\0';

    conn->ctx->p_charger->publish_EVCCIDD(buffer); // publish EVCC ID

    dlog(DLOG_LEVEL_INFO, "SessionSetupReq.EVCCID: %s", std::string(buffer).size() ? buffer : "(zero length provided)");

    /* un-arm a potentially communication setup timeout */
    stop_timer(&conn->ctx->com_setup_timeout, "session_setup: V2G_COMMUNICATION_SETUP_TIMER", conn->ctx);

    /* [V2G2-756]: If the SECC receives a SessionSetupReq including a SessionID value which is not
     * equal to zero (0) and not equal to the SessionID value stored from the preceding V2G
     * Communication Session, it shall send a SessionID value in the SessionSetupRes message that is
     * unequal to "0" and unequal to the SessionID value stored from the preceding V2G Communication
     * Session and indicate the new V2G Communication Session with the ResponseCode set to
     * "OK_NewSessionEstablished"
     */

    // TODO: handle resuming sessions [V2G2-463]

    /* Now fill the evse response message */
    res->ResponseCode = iso1responseCodeType_OK_NewSessionEstablished;

    /* Check and init session id */
    /* If no session id is configured, generate one */
    srand((unsigned int)time(NULL));
    if (conn->ctx->evse_v2g_data.session_id == (uint64_t)0 ||
        conn->ctx->evse_v2g_data.session_id != conn->ctx->ev_v2g_data.received_session_id) {
        conn->ctx->evse_v2g_data.session_id =
            ((uint64_t)rand() << 48) | ((uint64_t)rand() << 32) | ((uint64_t)rand() << 16) | (uint64_t)rand();
        dlog(
            DLOG_LEVEL_INFO,
            "No session_id found or not equal to the id from the preceding v2g session. Generating random session id.");
        dlog(DLOG_LEVEL_INFO, "Created new session with id 0x%08" PRIu64, conn->ctx->evse_v2g_data.session_id);
    } else {
        dlog(DLOG_LEVEL_INFO, "Found Session_id from the old session: 0x%08" PRIu64,
             conn->ctx->evse_v2g_data.session_id);
        res->ResponseCode = iso1responseCodeType_OK_OldSessionJoined;
    }

    /* TODO: publish EVCCID to MQTT */

    res->EVSEID.charactersLen = conn->ctx->evse_v2g_data.evse_id.bytesLen;
    memcpy(res->EVSEID.characters, conn->ctx->evse_v2g_data.evse_id.bytes, conn->ctx->evse_v2g_data.evse_id.bytesLen);

    res->EVSETimeStamp_isUsed = conn->ctx->evse_v2g_data.date_time_now_is_used;
    res->EVSETimeStamp = time(NULL);

    /* Check the current response code and check if no external error has occurred */
    next_event = (v2g_event)iso_validate_response_code(&res->ResponseCode, conn);

    /* Set next expected req msg */
    conn->ctx->state = (int)iso_dc_state_id::WAIT_FOR_SERVICEDISCOVERY; // [V2G-543]

    return next_event;
}

/*!
 * \brief handle_iso_service_discovery This function handles the din service discovery msg pair. It analyzes the request
 * msg and fills the response msg. The request and response msg based on the open V2G structures. This structures must
 * be provided within the \c conn structure.
 * \param conn holds the structure with the V2G msg pair.
 * \return Returns the next V2G-event.
 */
static enum v2g_event handle_iso_service_discovery(struct v2g_connection* conn) {
    struct iso1ServiceDiscoveryReqType* req = &conn->exi_in.iso1EXIDocument->V2G_Message.Body.ServiceDiscoveryReq;
    struct iso1ServiceDiscoveryResType* res = &conn->exi_out.iso1EXIDocument->V2G_Message.Body.ServiceDiscoveryRes;
    enum v2g_event nextEvent = V2G_EVENT_NO_EVENT;
    int8_t scope_idx = -1; // To find a list entry within the evse service list */

    /* At first, publish the received ev request message to the MQTT interface */
    publish_iso_service_discovery_req(req);

    /* build up response */
    res->ResponseCode = iso1responseCodeType_OK;

    // Checking of the charge service id
    if (conn->ctx->evse_v2g_data.charge_service.ServiceID != V2G_SERVICE_ID_CHARGING) {
        dlog(DLOG_LEVEL_WARNING,
             "Selected ServiceID is not ISO15118 conform. Correcting value to '1' (Charge service id)");
        conn->ctx->evse_v2g_data.charge_service.ServiceID = V2G_SERVICE_ID_CHARGING;
    }
    // Checking of the service category
    if (conn->ctx->evse_v2g_data.charge_service.ServiceCategory != iso1serviceCategoryType_EVCharging) {
        dlog(DLOG_LEVEL_WARNING,
             "Selected ServiceCategory is not ISO15118 conform. Correcting value to '0' (EVCharging)");
        conn->ctx->evse_v2g_data.charge_service.ServiceCategory = iso1serviceCategoryType_EVCharging;
    }

    res->ChargeService = conn->ctx->evse_v2g_data.charge_service;

    // Checking of the payment options
    if ((!conn->is_tls_connection) &&
        ((conn->ctx->evse_v2g_data.payment_option_list[0] == iso1paymentOptionType_Contract) ||
         (conn->ctx->evse_v2g_data.payment_option_list[1] == iso1paymentOptionType_Contract)) &&
        (false == conn->ctx->debugMode)) {
        conn->ctx->evse_v2g_data.payment_option_list[0] = iso1paymentOptionType_ExternalPayment;
        conn->ctx->evse_v2g_data.payment_option_list_len = 1;
        dlog(DLOG_LEVEL_WARNING,
             "PnC is not allowed without TLS-communication. Correcting value to '1' (ExternalPayment)");
    }

    memcpy(res->PaymentOptionList.PaymentOption.array, conn->ctx->evse_v2g_data.payment_option_list,
           conn->ctx->evse_v2g_data.payment_option_list_len * sizeof(iso1paymentOptionType));
    res->PaymentOptionList.PaymentOption.arrayLen = conn->ctx->evse_v2g_data.payment_option_list_len;

    /* Find requested scope id within evse service list */
    if (req->ServiceScope_isUsed) {
        /* Check if ServiceScope is in evse ServiceList */
        for (uint8_t idx = 0; idx < res->ServiceList.Service.arrayLen; idx++) {
            if ((res->ServiceList.Service.array[idx].ServiceScope_isUsed == (unsigned int)1) &&
                (strcmp(res->ServiceList.Service.array[idx].ServiceScope.characters, req->ServiceScope.characters) ==
                 0)) {
                scope_idx = idx;
                break;
            }
        }
    }

    /*  The SECC always returns all supported services for all scopes if no specific ServiceScope has been
        indicated in request message. */
    if (scope_idx == (int8_t)-1) {
        memcpy(res->ServiceList.Service.array, conn->ctx->evse_v2g_data.evse_service_list,
               sizeof(struct iso1ServiceType) * conn->ctx->evse_v2g_data.evse_service_list_len);
        res->ServiceList.Service.arrayLen = conn->ctx->evse_v2g_data.evse_service_list_len;
    } else {
        /* Offer only the requested ServiceScope entry */
        res->ServiceList.Service.array[0] = conn->ctx->evse_v2g_data.evse_service_list[scope_idx];
        res->ServiceList.Service.arrayLen = 1;
    }

    res->ServiceList_isUsed =
        ((uint16_t)0 < conn->ctx->evse_v2g_data.evse_service_list_len) ? (unsigned int)1 : (unsigned int)0;

    /* Check the current response code and check if no external error has occurred */
    nextEvent = (v2g_event)iso_validate_response_code(&res->ResponseCode, conn);

    /* Set next expected req msg */
    conn->ctx->state = (int)iso_dc_state_id::WAIT_FOR_SVCDETAIL_PAYMENTSVCSEL; // [V2G-545]

    return nextEvent;
}

/*!
 * \brief handle_iso_service_detail This function handles the iso_service_detail msg pair. It analyzes the request msg
 * and fills the response msg. The request and response msg based on the open V2G structures. This structures must be
 * provided within the \c conn structure. (Optional VAS)
 * \param conn holds the structure with the V2G msg pair.
 * \return Returns the next V2G-event.
 */
static enum v2g_event handle_iso_service_detail(struct v2g_connection* conn) {
    struct iso1ServiceDetailReqType* req = &conn->exi_in.iso1EXIDocument->V2G_Message.Body.ServiceDetailReq;
    struct iso1ServiceDetailResType* res = &conn->exi_out.iso1EXIDocument->V2G_Message.Body.ServiceDetailRes;
    enum v2g_event next_event = V2G_EVENT_NO_EVENT;

    /* At first, publish the received ev request message to the MQTT interface */
    publish_iso_service_detail_req(req);

    res->ResponseCode = iso1responseCodeType_OK;

    /* ServiceID reported back always matches the requested one */
    res->ServiceID = req->ServiceID;

    bool service_id_found = false;

    for (uint8_t idx = 0; idx < conn->ctx->evse_v2g_data.evse_service_list_len; idx++) {

        if (req->ServiceID == conn->ctx->evse_v2g_data.evse_service_list[idx].ServiceID) {
            service_id_found = true;

            /* Fill parameter list of the requested service id [V2G2-549] */
            for (uint8_t idx2 = 0; idx2 < conn->ctx->evse_v2g_data.service_parameter_list[idx].ParameterSet.arrayLen;
                 idx2++) {
                res->ServiceParameterList.ParameterSet.array[idx2] =
                    conn->ctx->evse_v2g_data.service_parameter_list[idx].ParameterSet.array[idx2];
            }
            res->ServiceParameterList.ParameterSet.arrayLen =
                conn->ctx->evse_v2g_data.service_parameter_list[idx].ParameterSet.arrayLen;
            res->ServiceParameterList_isUsed = (res->ServiceParameterList.ParameterSet.arrayLen != 0) ? 1 : 0;
        }
    }
    service_id_found = (req->ServiceID == V2G_SERVICE_ID_CHARGING) ? true : service_id_found;

    if (false == service_id_found) {
        res->ResponseCode = iso1responseCodeType_FAILED_ServiceIDInvalid; // [V2G2-464]
    }

    /* Check the current response code and check if no external error has occurred */
    next_event = (v2g_event)iso_validate_response_code(&res->ResponseCode, conn);

    /* Set next expected req msg */
    conn->ctx->state = (int)iso_dc_state_id::WAIT_FOR_SVCDETAIL_PAYMENTSVCSEL; // [V2G-DC-548]

    return next_event;
}

/*!
 * \brief handle_iso_payment_service_selection This function handles the iso_payment_service_selection msg pair. It
 * analyzes the request msg and fills the response msg. The request and response msg based on the open V2G structures.
 * This structures must be provided within the \c conn structure.
 * \param conn holds the structure with the V2G msg pair.
 * \return Returns the next V2G-event.
 */
static enum v2g_event handle_iso_payment_service_selection(struct v2g_connection* conn) {
    struct iso1PaymentServiceSelectionReqType* req =
        &conn->exi_in.iso1EXIDocument->V2G_Message.Body.PaymentServiceSelectionReq;
    struct iso1PaymentServiceSelectionResType* res =
        &conn->exi_out.iso1EXIDocument->V2G_Message.Body.PaymentServiceSelectionRes;
    enum v2g_event next_event = V2G_EVENT_NO_EVENT;
    uint8_t idx = 0;
    bool list_element_found = false;

    /* At first, publish the received ev request message to the customer mqtt interface */
    publish_iso_payment_service_selection_req(req);

    res->ResponseCode = iso1responseCodeType_OK;

    /* check whether the selected payment option was announced at all;
     * this also covers the case that the peer sends any invalid/unknown payment option
     * in the message; if we are not happy -> bail out
     */
    for (idx = 0; idx < conn->ctx->evse_v2g_data.payment_option_list_len; idx++) {
        if ((conn->ctx->evse_v2g_data.payment_option_list[idx] == req->SelectedPaymentOption)) {
            list_element_found = true;
            conn->ctx->p_charger->publish_SelectedPaymentOption(
                static_cast<types::iso15118_charger::PaymentOption>(req->SelectedPaymentOption));
            break;
        }
    }
    res->ResponseCode = (list_element_found == true)
                            ? res->ResponseCode
                            : iso1responseCodeType_FAILED_PaymentSelectionInvalid; // [V2G2-465]

    /* Check the selected services */
    bool charge_service_found = false;
    bool selected_services_found = true;

    for (uint8_t req_idx = 0;
         (req_idx < req->SelectedServiceList.SelectedService.arrayLen) && (selected_services_found == true);
         req_idx++) {

        /* Check if it's a charging service */
        if (req->SelectedServiceList.SelectedService.array[req_idx].ServiceID == V2G_SERVICE_ID_CHARGING) {
            charge_service_found = true;
        }
        /* Otherwise check if the selected service is in the stored in the service list */
        else {
            bool entry_found = false;
            for (uint8_t ci_idx = 0;
                 (ci_idx < conn->ctx->evse_v2g_data.evse_service_list_len) && (entry_found == false); ci_idx++) {

                if (req->SelectedServiceList.SelectedService.array[req_idx].ServiceID ==
                    conn->ctx->evse_v2g_data.evse_service_list[ci_idx].ServiceID) {
                    /* If it's stored, search for the next requested SelectedService entry */
                    dlog(DLOG_LEVEL_INFO, "Selected service id %i found",
                         conn->ctx->evse_v2g_data.evse_service_list[ci_idx].ServiceID);

                    if (conn->ctx->evse_v2g_data.evse_service_list[ci_idx].ServiceID == SAE_V2H) {
                        conn->ctx->evse_v2g_data.sae_bidi_data.enabled_sae_v2h = true;
                        conn->ctx->evse_v2g_data.sae_bidi_data.enabled_sae_v2g = false;
                        conn->ctx->p_charger->publish_sae_bidi_mode_active(nullptr);
                    } else if (conn->ctx->evse_v2g_data.evse_service_list[ci_idx].ServiceID == SAE_V2G) {
                        conn->ctx->evse_v2g_data.sae_bidi_data.enabled_sae_v2h = false;
                        conn->ctx->evse_v2g_data.sae_bidi_data.enabled_sae_v2g = true;
                        conn->ctx->p_charger->publish_sae_bidi_mode_active(nullptr);
                    }
                    entry_found = true;
                    break;
                }
            }
            if (entry_found == false) {
                /* If the requested SelectedService entry was not found, break up service list check */
                selected_services_found = false;
                break;
            }
        }
    }

    res->ResponseCode = (selected_services_found == false) ? iso1responseCodeType_FAILED_ServiceSelectionInvalid
                                                           : res->ResponseCode; // [V2G2-467]
    res->ResponseCode = (charge_service_found == false) ? iso1responseCodeType_FAILED_NoChargeServiceSelected
                                                        : res->ResponseCode; // [V2G2-804]

    /* Check the current response code and check if no external error has occurred */
    next_event = (v2g_event)iso_validate_response_code(&res->ResponseCode, conn);

    if (req->SelectedPaymentOption == iso1paymentOptionType_Contract) {
        dlog(DLOG_LEVEL_INFO, "SelectedPaymentOption: Contract");
        conn->ctx->session.iso_selected_payment_option = iso1paymentOptionType_Contract;
        /* Set next expected req msg */
        conn->ctx->state =
            (int)iso_dc_state_id::WAIT_FOR_PAYMENTDETAILS_CERTINST_CERTUPD; // [V2G-551] (iso specification describes
                                                                            // only the ac case... )
    } else {
        dlog(DLOG_LEVEL_INFO, "SelectedPaymentOption: ExternalPayment");
        conn->ctx->evse_v2g_data.evse_processing[PHASE_AUTH] =
            (uint8_t)iso1EVSEProcessingType_Ongoing_WaitingForCustomerInteraction; // [V2G2-854]
        /* Set next expected req msg */
        conn->ctx->state = (int)
            iso_dc_state_id::WAIT_FOR_AUTHORIZATION; // [V2G-551] (iso specification describes only the ac case... )
        conn->ctx->session.auth_start_timeout = getmonotonictime();
    }

    return next_event;
}

/*!
 * \brief handle_iso_payment_details This function handles the iso_payment_details msg pair. It analyzes the request msg
 * and fills the response msg. The request and response msg based on the open V2G structures. This structures must be
 * provided within the \c conn structure.
 * \param conn holds the structure with the V2G msg pair.
 * \return Returns the next V2G-event.
 */
static enum v2g_event handle_iso_payment_details(struct v2g_connection* conn) {
    struct iso1PaymentDetailsReqType* req = &conn->exi_in.iso1EXIDocument->V2G_Message.Body.PaymentDetailsReq;
    struct iso1PaymentDetailsResType* res = &conn->exi_out.iso1EXIDocument->V2G_Message.Body.PaymentDetailsRes;
    enum v2g_event nextEvent = V2G_EVENT_NO_EVENT;
    int err;

    // === For the contract certificate, the certificate chain should be checked ===
    conn->ctx->session.contract.valid_crt = false;

    if (conn->ctx->session.iso_selected_payment_option == iso1paymentOptionType_Contract) {
        // Free old stuff if it exists
        mbedtls_x509_crt_free(&conn->ctx->session.contract.crt);
        mbedtls_ecdsa_free(&conn->ctx->session.contract.pubkey);

        mbedtls_x509_crt_init(&conn->ctx->session.contract.crt);
        mbedtls_ecdsa_init(&conn->ctx->session.contract.pubkey);

        // Parse contract leaf certificate
        if (req->ContractSignatureCertChain.Certificate.bytesLen != 0) {
            err = mbedtls_x509_crt_parse(&conn->ctx->session.contract.crt,
                                         req->ContractSignatureCertChain.Certificate.bytes,
                                         req->ContractSignatureCertChain.Certificate.bytesLen);
        } else {
            dlog(DLOG_LEVEL_ERROR, "No certificate received!");
            res->ResponseCode = iso1responseCodeType_FAILED_CertChainError;
            goto error_out;
        }

        /* Check the received emaid against the certificate emaid. */
        char cert_emaid[iso1PaymentDetailsReqType_eMAID_CHARACTERS_SIZE];
        getEmaidFromContractCert(&conn->ctx->session.contract.crt.subject, cert_emaid, sizeof(cert_emaid));

        /* Filter '-' character */
        uint8_t emaid_len = 0;
        for (uint8_t idx = 0; idx < req->eMAID.charactersLen; idx++) {
            if ('-' != req->eMAID.characters[idx]) {
                req->eMAID.characters[emaid_len++] = req->eMAID.characters[idx];
            }
        }

        req->eMAID.charactersLen = emaid_len;
        req->eMAID.characters[emaid_len] = '\0';

        dlog(DLOG_LEVEL_TRACE, "emaid-v2g: %s emaid-cert: %s", req->eMAID.characters, cert_emaid);

        if ((std::string(cert_emaid).size() != req->eMAID.charactersLen) ||
            (0 != strncasecmp(req->eMAID.characters, cert_emaid, req->eMAID.charactersLen))) {
            dlog(DLOG_LEVEL_ERROR, "emaid of the contract certificate doesn't match with the received v2g-emaid");
            res->ResponseCode = iso1responseCodeType_FAILED_CertChainError;
            goto error_out;
        }

        if (err != 0) {
            memset(res, 0, sizeof(*res));
            res->ResponseCode = iso1responseCodeType_FAILED_CertChainError;
            char strerr[256];
            mbedtls_strerror(err, strerr, std::string(strerr).size());
            dlog(DLOG_LEVEL_ERROR, "handle_payment_detail: invalid certificate received in req: %s", strerr);
            goto error_out;
        }

        // Convert the public key in the certificate to a mbed TLS ECDSA public key
        // This also verifies that it's an ECDSA key and not an RSA key
        err = mbedtls_ecdsa_from_keypair(&conn->ctx->session.contract.pubkey,
                                         mbedtls_pk_ec(conn->ctx->session.contract.crt.pk));
        if (err != 0) {
            memset(res, 0, sizeof(*res));
            res->ResponseCode = iso1responseCodeType_FAILED_CertChainError;
            char strerr[256];
            mbedtls_strerror(err, strerr, std::string(strerr).size());
            dlog(DLOG_LEVEL_ERROR, "Could not retrieve ecdsa public key from certificate keypair: %s", strerr);
            goto error_out;
        }

        // Parse contract sub certificates
        if (req->ContractSignatureCertChain.SubCertificates_isUsed == 1) {
            for (int i = 0; i < req->ContractSignatureCertChain.SubCertificates.Certificate.arrayLen; i++) {
                err = mbedtls_x509_crt_parse(
                    &conn->ctx->session.contract.crt,
                    req->ContractSignatureCertChain.SubCertificates.Certificate.array[i].bytes,
                    req->ContractSignatureCertChain.SubCertificates.Certificate.array[i].bytesLen);

                if (err != 0) {
                    char strerr[256];
                    mbedtls_strerror(err, strerr, std::string(strerr).size());
                    dlog(DLOG_LEVEL_ERROR, "handle_payment_detail: invalid sub-certificate received in req: %s",
                         strerr);
                    goto error_out;
                }
            }
        }

        // initialize contract cert chain to retrieve ocsp request data
        std::string contract_cert_chain_pem = "";
        // Save the certificate chain in a variable in PEM format to publish it
        mbedtls_x509_crt* crt = &conn->ctx->session.contract.crt;
        unsigned char* base64Buffer = NULL;
        size_t olen;

        while (crt != nullptr && crt->version != 0) {
            mbedtls_base64_encode(NULL, 0, &olen, crt->raw.p, crt->raw.len);
            base64Buffer = static_cast<unsigned char*>(malloc(olen));
            if ((base64Buffer == NULL) ||
                ((mbedtls_base64_encode(base64Buffer, olen, &olen, crt->raw.p, crt->raw.len)) != 0)) {
                dlog(DLOG_LEVEL_ERROR, "Unable to encode certificate chain");
                break;
            }
            contract_cert_chain_pem.append("-----BEGIN CERTIFICATE-----\n");
            contract_cert_chain_pem.append(std::string(reinterpret_cast<char const*>(base64Buffer), olen));
            contract_cert_chain_pem.append("\n-----END CERTIFICATE-----\n");

            free(base64Buffer);
            crt = crt->next;
        }

        std::optional<std::vector<types::iso15118_charger::CertificateHashDataInfo>> iso15118_certificate_hash_data;

        /* Only if certificate chain verification should be done locally by the EVSE */
        if (conn->ctx->session.verify_contract_cert_chain == true) {
            std::string v2g_root_cert_path =
                conn->ctx->r_security->call_get_verify_file(types::evse_security::CaCertificateType::V2G);
            std::string mo_root_cert_path =
                conn->ctx->r_security->call_get_verify_file(types::evse_security::CaCertificateType::MO);
            mbedtls_x509_crt contract_root_crt;
            mbedtls_x509_crt_init(&contract_root_crt);
            uint32_t flags;

            /* Load supported V2G/MO root certificates */
            if (load_contract_root_cert(&contract_root_crt, v2g_root_cert_path.c_str(), mo_root_cert_path.c_str()) ==
                false) {
                memset(res, 0, sizeof(*res));
                res->ResponseCode = iso1responseCodeType_FAILED_NoCertificateAvailable;
                goto error_out;
            }
            // === Verify the retrieved contract ECDSA key against the root cert ===
            err = mbedtls_x509_crt_verify(&conn->ctx->session.contract.crt, &contract_root_crt, NULL, NULL, &flags,
                                          (conn->ctx->debugMode == true) ? debug_verify_cert : NULL, NULL);

            if (err != 0) {
                printMbedVerifyErrorCode(err, flags);
                memset(res, 0, sizeof(*res));
                dlog(DLOG_LEVEL_ERROR, "Validation of the contract certificate failed!");
                if ((err == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) && (flags & MBEDTLS_X509_BADCERT_EXPIRED)) {
                    res->ResponseCode = iso1responseCodeType_FAILED_CertificateExpired;
                } else if ((err == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) && (flags & MBEDTLS_X509_BADCERT_REVOKED)) {
                    res->ResponseCode = iso1responseCodeType_FAILED_CertificateRevoked;
                } else {
                    res->ResponseCode = iso1responseCodeType_FAILED_CertChainError;
                }
                // EVSETimeStamp and GenChallenge are mandatory, GenChallenge has fixed size
                res->EVSETimeStamp = time(NULL);
                memset(res->GenChallenge.bytes, 0, GEN_CHALLENGE_SIZE);
                res->GenChallenge.bytesLen = GEN_CHALLENGE_SIZE;
                goto error_out;
            }

            dlog(DLOG_LEVEL_INFO, "Validation of the contract certificate was successful!");

            // contract chain ocsp data can only be retrieved if the MO root is present and the chain could be verified
            const auto ocsp_response = conn->ctx->r_security->call_get_mo_ocsp_request_data(contract_cert_chain_pem);
            iso15118_certificate_hash_data = convert_to_certificate_hash_data_info_vector(ocsp_response);
        }

        generate_random_data(&conn->ctx->session.gen_challenge, GEN_CHALLENGE_SIZE);
        memcpy(res->GenChallenge.bytes, conn->ctx->session.gen_challenge, GEN_CHALLENGE_SIZE);
        res->GenChallenge.bytesLen = GEN_CHALLENGE_SIZE;
        conn->ctx->session.contract.valid_crt = true;

        // Publish the provided signature certificate chain and eMAID from EVCC
        // to receive PnC authorization
        types::authorization::ProvidedIdToken ProvidedIdToken;
        ProvidedIdToken.id_token = {std::string(cert_emaid), types::authorization::IdTokenType::eMAID};
        ProvidedIdToken.authorization_type = types::authorization::AuthorizationType::PlugAndCharge;
        ProvidedIdToken.iso15118CertificateHashData = iso15118_certificate_hash_data;
        ProvidedIdToken.certificate = contract_cert_chain_pem;
        conn->ctx->p_charger->publish_Require_Auth_PnC(ProvidedIdToken);

    } else {
        res->ResponseCode = iso1responseCodeType_FAILED;
        goto error_out;
    }
    res->EVSETimeStamp = time(NULL);
    res->ResponseCode = iso1responseCodeType_OK;

error_out:

    /* Check the current response code and check if no external error has occurred */
    nextEvent = iso_validate_response_code(&res->ResponseCode, conn);

    /* Set next expected req msg */
    conn->ctx->state = (int)iso_dc_state_id::WAIT_FOR_AUTHORIZATION; // [V2G-560]
    conn->ctx->session.auth_start_timeout = getmonotonictime();

    return nextEvent;
}

/*!
 * \brief handle_iso_authorization This function handles the iso_authorization msg pair. It analyzes the request msg and
 * fills the response msg. The request and response msg based on the open v2g structures. This structures must be
 * provided within the \c conn structure.
 * \param conn holds the structure with the v2g msg pair.
 * \return Returns the next v2g-event.
 */
static enum v2g_event handle_iso_authorization(struct v2g_connection* conn) {
    struct iso1AuthorizationReqType* req = &conn->exi_in.iso1EXIDocument->V2G_Message.Body.AuthorizationReq;
    struct iso1AuthorizationResType* res = &conn->exi_out.iso1EXIDocument->V2G_Message.Body.AuthorizationRes;
    enum v2g_event next_event = V2G_EVENT_NO_EVENT;
    struct timespec ts_abs_timeout;
    bool is_payment_option_contract = conn->ctx->session.iso_selected_payment_option == iso1paymentOptionType_Contract;

    /* At first, publish the received ev request message to the customer mqtt interface */
    publish_iso_authorization_req(req);

    res->ResponseCode = iso1responseCodeType_OK;

    if (conn->ctx->last_v2g_msg != V2G_AUTHORIZATION_MSG &&
        (conn->ctx->session.iso_selected_payment_option == iso1paymentOptionType_Contract)) { /* [V2G2-684] */
        if (req->GenChallenge_isUsed == 0 ||
            req->GenChallenge.bytesLen != 16 // [V2G2-697]  The GenChallenge field shall be exactly 128 bits long.
            || memcmp(req->GenChallenge.bytes, conn->ctx->session.gen_challenge, 16) != 0) {
            dlog(DLOG_LEVEL_ERROR, "Challenge invalid or not present");
            res->ResponseCode = iso1responseCodeType_FAILED_ChallengeInvalid; // [V2G2-475]
            goto error_out;
        }
        if (conn->exi_in.iso1EXIDocument->V2G_Message.Header.Signature_isUsed == 0) {
            dlog(DLOG_LEVEL_ERROR, "Missing signature (Signature_isUsed == 0)");
            res->ResponseCode = iso1responseCodeType_FAILED_SignatureError;
            goto error_out;
        }

        /* Validation of the received signature */
        struct iso1EXIFragment iso1_fragment;
        init_iso1EXIFragment(&iso1_fragment);

        iso1_fragment.AuthorizationReq_isUsed = 1u;
        memcpy(&iso1_fragment.AuthorizationReq, req, sizeof(*req));

        if (check_iso1_signature(&conn->exi_in.iso1EXIDocument->V2G_Message.Header.Signature,
                                 &conn->ctx->session.contract.pubkey, &iso1_fragment) == false) {
            res->ResponseCode = iso1responseCodeType_FAILED_SignatureError;
            goto error_out;
        }
    }
    res->EVSEProcessing = (iso1EVSEProcessingType)conn->ctx->evse_v2g_data.evse_processing[PHASE_AUTH];

    if (conn->ctx->evse_v2g_data.evse_processing[PHASE_AUTH] != iso1EVSEProcessingType_Finished) {
        if (((is_payment_option_contract == false) && (conn->ctx->session.auth_timeout_eim == 0)) ||
            ((is_payment_option_contract == true) && (conn->ctx->session.auth_timeout_pnc == 0))) {
            dlog(DLOG_LEVEL_DEBUG, "Waiting for authorization forever!");
        } else if ((getmonotonictime() - conn->ctx->session.auth_start_timeout) >=
                   1000 * (is_payment_option_contract ? conn->ctx->session.auth_timeout_pnc
                                                      : conn->ctx->session.auth_timeout_eim)) {
            conn->ctx->session.auth_start_timeout = getmonotonictime();
            res->ResponseCode = iso1responseCodeType_FAILED;
        }
    }

error_out:
    /* Check the current response code and check if no external error has occurred */
    next_event = (v2g_event)iso_validate_response_code(&res->ResponseCode, conn);

    /* Set next expected req msg */
    conn->ctx->state = (res->EVSEProcessing == iso1EVSEProcessingType_Finished)
                           ? (int)iso_dc_state_id::WAIT_FOR_CHARGEPARAMETERDISCOVERY
                           : (int)iso_dc_state_id::WAIT_FOR_AUTHORIZATION; // [V2G-573] (AC) , [V2G-687] (DC)

    return next_event;
}

/*!
 * \brief handle_iso_charge_parameter_discovery This function handles the iso_charge_parameter_discovery msg pair. It
 * analyzes the request msg and fills the response msg. The request and response msg based on the open V2G structures.
 * This structures must be provided within the \c conn structure.
 * \param conn holds the structure with the V2G msg pair.
 * \return Returns the next V2G-event.
 */
static enum v2g_event handle_iso_charge_parameter_discovery(struct v2g_connection* conn) {
    struct iso1ChargeParameterDiscoveryReqType* req =
        &conn->exi_in.iso1EXIDocument->V2G_Message.Body.ChargeParameterDiscoveryReq;
    struct iso1ChargeParameterDiscoveryResType* res =
        &conn->exi_out.iso1EXIDocument->V2G_Message.Body.ChargeParameterDiscoveryRes;
    enum v2g_event next_event = V2G_EVENT_NO_EVENT;
    struct timespec ts_abs_timeout;

    /* At first, publish the received ev request message to the MQTT interface */
    publish_iso_charge_parameter_discovery_req(conn->ctx, req);

    /* First, check requested energy transfer mode, because this information is necessary for futher configuration */
    res->ResponseCode = iso1responseCodeType_FAILED_WrongEnergyTransferMode;
    for (uint8_t idx = 0;
         idx < conn->ctx->evse_v2g_data.charge_service.SupportedEnergyTransferMode.EnergyTransferMode.arrayLen; idx++) {
        if (req->RequestedEnergyTransferMode ==
            conn->ctx->evse_v2g_data.charge_service.SupportedEnergyTransferMode.EnergyTransferMode.array[idx]) {
            res->ResponseCode = iso1responseCodeType_OK; // [V2G2-476]
            log_selected_energy_transfer_type((int)req->RequestedEnergyTransferMode);
            break;
        }
    }

    res->EVSEChargeParameter_isUsed = 0;
    res->EVSEProcessing = (iso1EVSEProcessingType)conn->ctx->evse_v2g_data.evse_processing[PHASE_PARAMETER];

    /* Configure SA-schedules*/
    if (res->EVSEProcessing == iso1EVSEProcessingType_Finished) {
        /* If processing is finished, configure SASchedule list */
        if (conn->ctx->evse_v2g_data.evse_sa_schedule_list_is_used == false) {
            /* If not configured, configure SA-schedule automatically for AC charging */
            if (conn->ctx->is_dc_charger == false) {
                /* Determin max current and nominal voltage */
                float max_current = conn->ctx->basic_config.evse_ac_current_limit;
                int64_t nom_voltage =
                    conn->ctx->evse_v2g_data.evse_nominal_voltage.Value *
                    pow(10, conn->ctx->evse_v2g_data.evse_nominal_voltage.Multiplier); /* nominal voltage */

                /* Calculate pmax based on max current, nominal voltage and phase count (which the car has selected
                 * above) */
                int64_t pmax =
                    max_current * nom_voltage *
                    ((req->RequestedEnergyTransferMode == iso1EnergyTransferModeType_AC_single_phase_core) ? 1 : 3);
                populate_physical_value(&conn->ctx->evse_v2g_data.evse_sa_schedule_list.SAScheduleTuple.array[0]
                                             .PMaxSchedule.PMaxScheduleEntry.array[0]
                                             .PMax,
                                        pmax, iso1unitSymbolType_W);
            } else {
                conn->ctx->evse_v2g_data.evse_sa_schedule_list.SAScheduleTuple.array[0]
                    .PMaxSchedule.PMaxScheduleEntry.array[0]
                    .PMax = conn->ctx->evse_v2g_data.evse_maximum_power_limit;
            }
            conn->ctx->evse_v2g_data.evse_sa_schedule_list.SAScheduleTuple.array[0]
                .PMaxSchedule.PMaxScheduleEntry.array[0]
                .RelativeTimeInterval.start = 0;
            conn->ctx->evse_v2g_data.evse_sa_schedule_list.SAScheduleTuple.array[0]
                .PMaxSchedule.PMaxScheduleEntry.array[0]
                .RelativeTimeInterval.duration_isUsed = 1;
            conn->ctx->evse_v2g_data.evse_sa_schedule_list.SAScheduleTuple.array[0]
                .PMaxSchedule.PMaxScheduleEntry.array[0]
                .RelativeTimeInterval.duration = SA_SCHEDULE_DURATION;
            conn->ctx->evse_v2g_data.evse_sa_schedule_list.SAScheduleTuple.array[0]
                .PMaxSchedule.PMaxScheduleEntry.arrayLen = 1;
            conn->ctx->evse_v2g_data.evse_sa_schedule_list.SAScheduleTuple.arrayLen = 1;
        }

        res->SAScheduleList = conn->ctx->evse_v2g_data.evse_sa_schedule_list;
        res->SAScheduleList_isUsed = (unsigned int)1; //  The SECC shall only omit the parameter 'SAScheduleList' in
                                                      //  case EVSEProcessing is set to 'Ongoing'.

        if ((req->MaxEntriesSAScheduleTuple_isUsed == (unsigned int)1) &&
            (req->MaxEntriesSAScheduleTuple < res->SAScheduleList.SAScheduleTuple.arrayLen)) {
            dlog(DLOG_LEVEL_WARNING, "EV's max. SA-schedule-tuple entries exceeded");
        }
    } else {
        res->EVSEProcessing = iso1EVSEProcessingType_Ongoing;
        res->SAScheduleList_isUsed = (unsigned int)0;
    }

    /* Checking SAScheduleTupleID */
    for (uint8_t idx = 0; idx < res->SAScheduleList.SAScheduleTuple.arrayLen; idx++) {
        if (res->SAScheduleList.SAScheduleTuple.array[idx].SAScheduleTupleID == (uint8_t)0) {
            dlog(DLOG_LEVEL_WARNING, "Selected SAScheduleTupleID is not ISO15118 conform. The SECC shall use the "
                                     "values 1 to 255"); // [V2G2-773]  The SECC shall use the values 1 to 255 for the
                                                         // parameter SAScheduleTupleID.
        }
    }

    res->SASchedules_isUsed = 0;

    // TODO: For DC charging wait for CP state B , before transmitting of the response ([V2G2-921], [V2G2-922]). CP
    // state is checked by other module

    /* reset our internal reminder that renegotiation was requested */
    conn->ctx->session.renegotiation_required = false; // Reset renegotiation flag

    if (conn->ctx->is_dc_charger == false) {
        /* Configure AC stucture elements */
        res->AC_EVSEChargeParameter_isUsed = 1;
        res->DC_EVSEChargeParameter_isUsed = 0;

        populate_ac_evse_status(conn->ctx, &res->AC_EVSEChargeParameter.AC_EVSEStatus);

        /* Max current */
        float max_current = conn->ctx->basic_config.evse_ac_current_limit;
        populate_physical_value_float(&res->AC_EVSEChargeParameter.EVSEMaxCurrent, max_current, 1,
                                      iso1unitSymbolType_A);

        /* Nominal voltage */
        res->AC_EVSEChargeParameter.EVSENominalVoltage = conn->ctx->evse_v2g_data.evse_nominal_voltage;
        int64_t nom_voltage = conn->ctx->evse_v2g_data.evse_nominal_voltage.Value *
                              pow(10, conn->ctx->evse_v2g_data.evse_nominal_voltage.Multiplier);

        /* Calculate pmax based on max current, nominal voltage and phase count (which the car has selected above) */
        int64_t pmax = max_current * nom_voltage *
                       ((iso1EnergyTransferModeType_AC_single_phase_core == req->RequestedEnergyTransferMode) ? 1 : 3);

        /* Check the SASchedule */
        if (res->SAScheduleList_isUsed == (unsigned int)1) {
            for (uint8_t idx = 0; idx < res->SAScheduleList.SAScheduleTuple.arrayLen; idx++) {
                for (uint8_t idx2 = 0;
                     idx2 < res->SAScheduleList.SAScheduleTuple.array[idx].PMaxSchedule.PMaxScheduleEntry.arrayLen;
                     idx2++)
                    if ((res->SAScheduleList.SAScheduleTuple.array[idx]
                             .PMaxSchedule.PMaxScheduleEntry.array[idx2]
                             .PMax.Value *
                         pow(10, res->SAScheduleList.SAScheduleTuple.array[idx]
                                     .PMaxSchedule.PMaxScheduleEntry.array[idx2]
                                     .PMax.Multiplier)) > pmax) {
                        dlog(DLOG_LEVEL_WARNING,
                             "Provided SA-schedule-list doesn't match with the physical value limits");
                    }
            }
        }

        if (req->DC_EVChargeParameter_isUsed == (unsigned int)1) {
            res->ResponseCode = iso1responseCodeType_FAILED_WrongChargeParameter; // [V2G2-477]
        }
    } else {

        if (conn->ctx->evse_v2g_data.sae_bidi_data.enabled_sae_v2h == true) {
            static bool first_req = true;

            if (first_req == true) {
                res->EVSEProcessing = iso1EVSEProcessingType_Ongoing;
                first_req = false;
            } else {
                // Check if second req message contains neg values
                // Check if bulk soc is set
                if (req->DC_EVChargeParameter.BulkSOC_isUsed == 1 &&
                    req->DC_EVChargeParameter.EVMaximumCurrentLimit.Value < 0 &&
                    req->DC_EVChargeParameter.EVMaximumPowerLimit_isUsed == 1 &&
                    req->DC_EVChargeParameter.EVMaximumPowerLimit.Value < 0) {
                    // Save bulk soc for minimal soc to stop
                    conn->ctx->evse_v2g_data.sae_bidi_data.sae_v2h_minimal_soc = req->DC_EVChargeParameter.BulkSOC;
                } else {
                    res->ResponseCode = iso1responseCodeType::iso1responseCodeType_FAILED_WrongEnergyTransferMode;
                }
                res->EVSEProcessing = iso1EVSEProcessingType_Finished;
                // reset first_req
                first_req = true;
            }
        }

        /* Configure DC stucture elements */
        res->DC_EVSEChargeParameter_isUsed = 1;
        res->AC_EVSEChargeParameter_isUsed = 0;

        res->DC_EVSEChargeParameter.DC_EVSEStatus.EVSEIsolationStatus =
            (iso1isolationLevelType)conn->ctx->evse_v2g_data.evse_isolation_status;
        res->DC_EVSEChargeParameter.DC_EVSEStatus.EVSEIsolationStatus_isUsed =
            conn->ctx->evse_v2g_data.evse_isolation_status_is_used;
        res->DC_EVSEChargeParameter.DC_EVSEStatus.EVSENotification =
            (iso1EVSENotificationType)conn->ctx->evse_v2g_data.evse_notification;
        res->DC_EVSEChargeParameter.DC_EVSEStatus.EVSEStatusCode =
            (iso1DC_EVSEStatusCodeType)conn->ctx->evse_v2g_data.evse_status_code[PHASE_PARAMETER];
        res->DC_EVSEChargeParameter.DC_EVSEStatus.NotificationMaxDelay =
            (uint16_t)conn->ctx->evse_v2g_data.notification_max_delay;

        res->DC_EVSEChargeParameter.EVSECurrentRegulationTolerance =
            conn->ctx->evse_v2g_data.evse_current_regulation_tolerance;
        res->DC_EVSEChargeParameter.EVSECurrentRegulationTolerance_isUsed =
            conn->ctx->evse_v2g_data.evse_current_regulation_tolerance_is_used;
        res->DC_EVSEChargeParameter.EVSEEnergyToBeDelivered = conn->ctx->evse_v2g_data.evse_energy_to_be_delivered;
        res->DC_EVSEChargeParameter.EVSEEnergyToBeDelivered_isUsed =
            conn->ctx->evse_v2g_data.evse_energy_to_be_delivered_is_used;
        res->DC_EVSEChargeParameter.EVSEMaximumCurrentLimit = conn->ctx->evse_v2g_data.evse_maximum_current_limit;
        res->DC_EVSEChargeParameter.EVSEMaximumPowerLimit = conn->ctx->evse_v2g_data.evse_maximum_power_limit;
        res->DC_EVSEChargeParameter.EVSEMaximumVoltageLimit = conn->ctx->evse_v2g_data.evse_maximum_voltage_limit;
        res->DC_EVSEChargeParameter.EVSEMinimumCurrentLimit = conn->ctx->evse_v2g_data.evse_minimum_current_limit;
        res->DC_EVSEChargeParameter.EVSEMinimumVoltageLimit = conn->ctx->evse_v2g_data.evse_minimum_voltage_limit;
        res->DC_EVSEChargeParameter.EVSEPeakCurrentRipple = conn->ctx->evse_v2g_data.evse_peak_current_ripple;

        if ((unsigned int)1 == req->AC_EVChargeParameter_isUsed) {
            res->ResponseCode = iso1responseCodeType_FAILED_WrongChargeParameter; // [V2G2-477]
        }
    }

    /* Check the current response code and check if no external error has occurred */
    next_event = (v2g_event)iso_validate_response_code(&res->ResponseCode, conn);

    /* Set next expected req msg */
    if (conn->ctx->is_dc_charger == true) {
        conn->ctx->state = (iso1EVSEProcessingType_Finished == res->EVSEProcessing)
                               ? (int)iso_dc_state_id::WAIT_FOR_CABLECHECK
                               : (int)iso_dc_state_id::WAIT_FOR_CHARGEPARAMETERDISCOVERY; // [V2G-582], [V2G-688]
    } else {
        conn->ctx->state = (iso1EVSEProcessingType_Finished == res->EVSEProcessing)
                               ? (int)iso_ac_state_id::WAIT_FOR_POWERDELIVERY
                               : (int)iso_ac_state_id::WAIT_FOR_CHARGEPARAMETERDISCOVERY;
    }

    return next_event;
}

/*!
 * \brief handle_iso_power_delivery This function handles the iso_power_delivery msg pair. It analyzes the request msg
 * and fills the response msg. The request and response msg based on the open V2G structures. This structures must be
 * provided within the \c conn structure.
 * \param conn holds the structure with the V2G msg pair.
 * \return Returns the next V2G-event.
 */
static enum v2g_event handle_iso_power_delivery(struct v2g_connection* conn) {
    struct iso1PowerDeliveryReqType* req = &conn->exi_in.iso1EXIDocument->V2G_Message.Body.PowerDeliveryReq;
    struct iso1PowerDeliveryResType* res = &conn->exi_out.iso1EXIDocument->V2G_Message.Body.PowerDeliveryRes;
    struct timespec ts_abs_timeout;
    uint8_t sa_schedule_tuple_idx = 0;
    bool entry_found = false;
    enum v2g_event next_event = V2G_EVENT_NO_EVENT;

    /* At first, publish the received EV request message to the MQTT interface */
    publish_iso_power_delivery_req(conn->ctx, req);

    /* build up response */
    res->ResponseCode = iso1responseCodeType_OK;

    switch (req->ChargeProgress) {
    case iso1chargeProgressType_Start:
        conn->ctx->p_charger->publish_V2G_Setup_Finished(nullptr);

        if (conn->ctx->is_dc_charger == false) {
            // TODO: For AC charging wait for CP state C or D , before transmitting of the response. CP state is checked
            // by other module
            if (conn->ctx->contactor_is_closed == false) {
                // TODO: Signal closing contactor with MQTT if no timeout while waiting for state C or D
                conn->ctx->p_charger->publish_AC_Close_Contactor(nullptr);
                conn->ctx->session.is_charging = true;

                /* determine timeout for contactor */
                clock_gettime(CLOCK_MONOTONIC, &ts_abs_timeout);
                timespec_add_ms(&ts_abs_timeout, V2G_CONTACTOR_CLOSE_TIMEOUT);

                /* wait for contactor to really close or timeout */
                dlog(DLOG_LEVEL_INFO, "Waiting for contactor is closed");

                int rv = 0;
                while ((rv == 0) && (conn->ctx->contactor_is_closed == false) &&
                       (conn->ctx->intl_emergency_shutdown == false) && (conn->ctx->stop_hlc == false) &&
                       (conn->ctx->is_connection_terminated == false)) {
                    pthread_mutex_lock(&conn->ctx->mqtt_lock);
                    rv = pthread_cond_timedwait(&conn->ctx->mqtt_cond, &conn->ctx->mqtt_lock, &ts_abs_timeout);
                    if (rv == EINTR)
                        rv = 0; /* restart */
                    if (rv == ETIMEDOUT) {
                        dlog(DLOG_LEVEL_ERROR, "timeout while waiting for contactor to close, signaling error");
                        res->ResponseCode = iso1responseCodeType_FAILED_ContactorError;
                    }
                    pthread_mutex_unlock(&conn->ctx->mqtt_lock);
                }
            }
        }
        break;

    case iso1chargeProgressType_Stop:
        conn->ctx->session.is_charging = false;

        if (conn->ctx->is_dc_charger == false) {
            // TODO: For AC charging wait for CP state change from C/D to B , before transmitting of the response. CP
            // state is checked by other module
            conn->ctx->p_charger->publish_AC_Open_Contactor(nullptr);
        } else {
            conn->ctx->p_charger->publish_currentDemand_Finished(nullptr);
            conn->ctx->p_charger->publish_DC_Open_Contactor(nullptr);
        }
        break;

    case iso1chargeProgressType_Renegotiate:
        conn->ctx->session.renegotiation_required = true;
        break;

    default:
        dlog(DLOG_LEVEL_ERROR, "Unknown ChargeProgress %d received, signaling error", req->ChargeProgress);
        res->ResponseCode = iso1responseCodeType_FAILED;
    }

    if (conn->ctx->is_dc_charger == false) {
        res->AC_EVSEStatus_isUsed = 1;
        res->DC_EVSEStatus_isUsed = 0;
        populate_ac_evse_status(conn->ctx, &res->AC_EVSEStatus);
    } else {
        res->DC_EVSEStatus_isUsed = 1;
        res->AC_EVSEStatus_isUsed = 0;
        res->DC_EVSEStatus.EVSEIsolationStatus = (iso1isolationLevelType)conn->ctx->evse_v2g_data.evse_isolation_status;
        res->DC_EVSEStatus.EVSEIsolationStatus_isUsed = conn->ctx->evse_v2g_data.evse_isolation_status_is_used;
        res->DC_EVSEStatus.EVSENotification = (iso1EVSENotificationType)conn->ctx->evse_v2g_data.evse_notification;
        res->DC_EVSEStatus.EVSEStatusCode =
            (iso1DC_EVSEStatusCodeType)conn->ctx->evse_v2g_data.evse_status_code[PHASE_CHARGE];
        res->DC_EVSEStatus.NotificationMaxDelay = (uint16_t)conn->ctx->evse_v2g_data.notification_max_delay;

        res->ResponseCode = (req->ChargeProgress == iso1chargeProgressType_Start) &&
                                    (res->DC_EVSEStatus.EVSEStatusCode != iso1DC_EVSEStatusCodeType_EVSE_Ready)
                                ? iso1responseCodeType_FAILED_PowerDeliveryNotApplied
                                : res->ResponseCode; // [V2G2-480]
    }

    res->EVSEStatus_isUsed = 0;

    /* Check the selected SAScheduleTupleID */
    for (sa_schedule_tuple_idx = 0;
         sa_schedule_tuple_idx < conn->ctx->evse_v2g_data.evse_sa_schedule_list.SAScheduleTuple.arrayLen;
         sa_schedule_tuple_idx++) {
        if ((conn->ctx->evse_v2g_data.evse_sa_schedule_list.SAScheduleTuple.array[sa_schedule_tuple_idx]
                 .SAScheduleTupleID == req->SAScheduleTupleID)) {
            entry_found = true;
            conn->ctx->session.sa_schedule_tuple_id = req->SAScheduleTupleID;
            break;
        }
    }

    res->ResponseCode =
        (entry_found == false) ? iso1responseCodeType_FAILED_TariffSelectionInvalid : res->ResponseCode; // [V2G2-479]

    /* Check EV charging profile values [V2G2-478] */
    check_iso1_charging_profile_values(req, res, conn, sa_schedule_tuple_idx);

    /* Check the current response code and check if no external error has occurred */
    next_event = (v2g_event)iso_validate_response_code(&res->ResponseCode, conn);

    /* Set next expected req msg */
    if ((req->ChargeProgress == iso1chargeProgressType_Renegotiate) &&
        ((conn->ctx->last_v2g_msg == V2G_CURRENT_DEMAND_MSG) || (conn->ctx->last_v2g_msg == V2G_CHARGING_STATUS_MSG))) {
        conn->ctx->state = (int)iso_dc_state_id::WAIT_FOR_CHARGEPARAMETERDISCOVERY; // [V2G-813]

        if (conn->ctx->is_dc_charger == false) {
            // Reset AC relevant parameter to start the renegotation process
            conn->ctx->evse_v2g_data.evse_notification =
                (conn->ctx->evse_v2g_data.evse_notification == iso1EVSENotificationType_ReNegotiation)
                    ? iso1EVSENotificationType_None
                    : conn->ctx->evse_v2g_data.evse_notification;
        } else {
            // Reset DC relevant parameter to start the renegotation process
            conn->ctx->evse_v2g_data.evse_processing[PHASE_ISOLATION] = iso1EVSEProcessingType_Ongoing;
            conn->ctx->evse_v2g_data.evse_notification =
                (iso1EVSENotificationType_ReNegotiation == conn->ctx->evse_v2g_data.evse_notification)
                    ? iso1EVSENotificationType_None
                    : conn->ctx->evse_v2g_data.evse_notification;
            conn->ctx->evse_v2g_data.evse_isolation_status = iso1isolationLevelType_Invalid;
        }
    } else if ((req->ChargeProgress == iso1chargeProgressType_Start) &&
               (conn->ctx->last_v2g_msg != V2G_CURRENT_DEMAND_MSG) &&
               (conn->ctx->last_v2g_msg != V2G_CHARGING_STATUS_MSG)) {
        conn->ctx->state = (conn->ctx->is_dc_charger == true)
                               ? (int)iso_dc_state_id::WAIT_FOR_CURRENTDEMAND
                               : (int)iso_ac_state_id::WAIT_FOR_CHARGINGSTATUS; // [V2G-590], [V2G2-576]
    } else {
        /* abort charging session if EV is ready to charge after current demand phase */
        if (req->ChargeProgress != iso1chargeProgressType_Stop) {
            res->ResponseCode = iso1responseCodeType_FAILED; // (/*[V2G2-812]*/
        }
        conn->ctx->state = (conn->ctx->is_dc_charger == true)
                               ? (int)iso_dc_state_id::WAIT_FOR_WELDINGDETECTION_SESSIONSTOP
                               : (int)iso_ac_state_id::WAIT_FOR_SESSIONSTOP; // [V2G-601], [V2G2-568]
    }

    return next_event;
}

/*!
 * \brief handle_iso_charging_status This function handles the iso_charging_status msg pair. It analyzes the request msg
 * and fills the response msg. The request and response msg based on the open V2G structures. This structures must be
 * provided within the \c conn structure.
 * \param conn holds the structure with the V2G msg pair.
 * \return Returns the next V2G-event.
 */
static enum v2g_event handle_iso_charging_status(struct v2g_connection* conn) {
    struct iso1ChargingStatusResType* res = &conn->exi_out.iso1EXIDocument->V2G_Message.Body.ChargingStatusRes;
    enum v2g_event next_event = V2G_EVENT_NO_EVENT;
    /* build up response */
    res->ResponseCode = iso1responseCodeType_OK;

    res->ReceiptRequired = conn->ctx->evse_v2g_data.receipt_required;
    res->ReceiptRequired_isUsed =
        (conn->ctx->session.iso_selected_payment_option == iso1paymentOptionType_Contract) ? 1U : 0U;

    if (conn->ctx->meter_info.meter_info_is_used == true) {
        res->MeterInfo.MeterID.charactersLen = conn->ctx->meter_info.meter_id.bytesLen;
        memcpy(res->MeterInfo.MeterID.characters, conn->ctx->meter_info.meter_id.bytes,
               iso1MeterInfoType_MeterID_CHARACTERS_SIZE);
        res->MeterInfo.MeterReading = conn->ctx->meter_info.meter_reading;
        res->MeterInfo.MeterReading_isUsed = 1;
        res->MeterInfo_isUsed = 1;
        // Reset the signal for the next time handle_set_MeterInfo is signaled
        conn->ctx->meter_info.meter_info_is_used = false;
    } else {
        res->MeterInfo_isUsed = 0;
    }

    res->EVSEMaxCurrent_isUsed = (conn->ctx->session.iso_selected_payment_option == iso1paymentOptionType_Contract)
                                     ? (unsigned int)0
                                     : (unsigned int)1; // This element is not included in the message if any AC PnC
                                                        // Message Set has been selected.

    if ((unsigned int)1 == res->EVSEMaxCurrent_isUsed) {
        populate_physical_value_float(&res->EVSEMaxCurrent, conn->ctx->basic_config.evse_ac_current_limit, 1,
                                      iso1unitSymbolType_A);
    }

    conn->exi_out.iso1EXIDocument->V2G_Message.Body.ChargingStatusRes_isUsed = 1;

    /* the following field can also be set in error path */
    res->EVSEID.charactersLen = conn->ctx->evse_v2g_data.evse_id.bytesLen;
    memcpy(res->EVSEID.characters, conn->ctx->evse_v2g_data.evse_id.bytes, conn->ctx->evse_v2g_data.evse_id.bytesLen);

    /* in error path the session might not be available */
    res->SAScheduleTupleID = conn->ctx->session.sa_schedule_tuple_id;
    populate_ac_evse_status(conn->ctx, &res->AC_EVSEStatus);

    /* Check the current response code and check if no external error has occurred */
    next_event = (enum v2g_event)iso_validate_response_code(&res->ResponseCode, conn);

    /* Set next expected req msg */
    conn->ctx->state = (((int)1 == res->ReceiptRequired))
                           ? (int)iso_ac_state_id::WAIT_FOR_METERINGRECEIPT
                           : (int)iso_ac_state_id::WAIT_FOR_CHARGINGSTATUS_POWERDELIVERY; // [V2G2-577], [V2G2-575]

    return next_event;
}

/*!
 * \brief handle_iso_metering_receipt This function handles the iso_metering_receipt msg pair. It analyzes the request
 * msg and fills the response msg. The request and response msg based on the open V2G structures. This structures must
 * be provided within the \c conn structure. \param conn holds the structure with the V2G msg pair. \return Returns the
 * next V2G-event.
 */
static enum v2g_event handle_iso_metering_receipt(struct v2g_connection* conn) {
    struct iso1MeteringReceiptReqType* req = &conn->exi_in.iso1EXIDocument->V2G_Message.Body.MeteringReceiptReq;
    struct iso1MeteringReceiptResType* res = &conn->exi_out.iso1EXIDocument->V2G_Message.Body.MeteringReceiptRes;
    enum v2g_event next_event = V2G_EVENT_NO_EVENT;

    /* At first, publish the received ev request message to the MQTTinterface */
    publish_iso_metering_receipt_req(req);

    dlog(DLOG_LEVEL_TRACE, "EVSE side: meteringReceipt called");
    dlog(DLOG_LEVEL_TRACE, "\tReceived data:");

    dlog(DLOG_LEVEL_TRACE, "\t\t ID=%c%c%c", req->Id.characters[0], req->Id.characters[1], req->Id.characters[2]);
    dlog(DLOG_LEVEL_TRACE, "\t\t SAScheduleTupleID=%d", req->SAScheduleTupleID);
    dlog(DLOG_LEVEL_TRACE, "\t\t SessionID=%d", req->SessionID.bytes[1]);
    dlog(DLOG_LEVEL_TRACE, "\t\t MeterInfo.MeterStatus=%d", req->MeterInfo.MeterStatus);
    dlog(DLOG_LEVEL_TRACE, "\t\t MeterInfo.MeterID=%d", req->MeterInfo.MeterID.characters[0]);
    dlog(DLOG_LEVEL_TRACE, "\t\t MeterInfo.isused.MeterReading=%d", req->MeterInfo.MeterReading_isUsed);
    dlog(DLOG_LEVEL_TRACE, "\t\t MeterReading.Value=%lu", (long unsigned int)req->MeterInfo.MeterReading);
    dlog(DLOG_LEVEL_TRACE, "\t\t MeterInfo.TMeter=%li", (long int)req->MeterInfo.TMeter);

    res->ResponseCode = iso1responseCodeType_OK;

    if (conn->ctx->is_dc_charger == false) {
        /* for AC charging we respond with AC_EVSEStatus */
        res->EVSEStatus_isUsed = 0;
        res->AC_EVSEStatus_isUsed = 1;
        res->DC_EVSEStatus_isUsed = 0;
        populate_ac_evse_status(conn->ctx, &res->AC_EVSEStatus);
    } else {
        res->DC_EVSEStatus_isUsed = 1;
        res->AC_EVSEStatus_isUsed = 0;
    }

    /* Check the current response code and check if no external error has occurred */
    next_event = (v2g_event)iso_validate_response_code(&res->ResponseCode, conn);

    /* Set next expected req msg */
    conn->ctx->state = (conn->ctx->is_dc_charger == false)
                           ? (int)iso_ac_state_id::WAIT_FOR_CHARGINGSTATUS_POWERDELIVERY
                           : (int)iso_dc_state_id::WAIT_FOR_CURRENTDEMAND_POWERDELIVERY; // [V2G2-580]/[V2G-797]

    return next_event;
}

/*!
 * \brief handle_iso_certificate_update This function handles the iso_certificate_update msg pair. It analyzes the
 * request msg and fills the response msg. The request and response msg based on the open V2G structures. This
 * structures must be provided within the \c conn structure.
 * \param conn holds the structure with the V2G msg pair.
 * \return Returns the next V2G-event.
 */
static enum v2g_event handle_iso_certificate_update(struct v2g_connection* conn) {
    // TODO: implement CertificateUpdate handling
    return V2G_EVENT_NO_EVENT;
}

/*!
 * \brief handle_iso_certificate_installation This function handles the iso_certificate_installation msg pair. It
 * analyzes the request msg and fills the response msg. The request and response msg based on the open V2G structures.
 * This structures must be provided within the \c conn structure.
 * \param conn holds the structure with the V2G msg pair.
 * \return Returns the next V2G-event.
 */
static enum v2g_event handle_iso_certificate_installation(struct v2g_connection* conn) {
    struct iso1CertificateInstallationResType* res =
        &conn->exi_out.iso1EXIDocument->V2G_Message.Body.CertificateInstallationRes;
    enum v2g_event nextEvent = V2G_EVENT_SEND_AND_TERMINATE;
    struct timespec ts_abs_timeout;
    int rv = 0;
    /* At first, publish the received EV request message to the customer MQTT interface */
    if (publish_iso_certificate_installation_exi_req(conn->ctx, conn->buffer + V2GTP_HEADER_LENGTH,
                                                     conn->stream.size - V2GTP_HEADER_LENGTH) == false) {
        dlog(DLOG_LEVEL_ERROR, "Failed to send CertificateInstallationExiReq");
        goto exit;
    }
    /* Waiting for the CertInstallationExiRes msg */
    clock_gettime(CLOCK_MONOTONIC, &ts_abs_timeout);
    timespec_add_ms(&ts_abs_timeout, V2G_SECC_MSG_CERTINSTALL_TIME);
    dlog(DLOG_LEVEL_INFO, "Waiting for the CertInstallationExiRes msg");
    while ((rv == 0) && (conn->ctx->evse_v2g_data.cert_install_res_b64_buffer.empty() == true) &&
           (conn->ctx->intl_emergency_shutdown == false) && (conn->ctx->stop_hlc == false) &&
           (conn->ctx->is_connection_terminated == false)) { // [V2G2-917]
        pthread_mutex_lock(&conn->ctx->mqtt_lock);
        rv = pthread_cond_timedwait(&conn->ctx->mqtt_cond, &conn->ctx->mqtt_lock, &ts_abs_timeout);
        if (rv == EINTR)
            rv = 0; /* restart */
        if (rv == ETIMEDOUT) {
            dlog(DLOG_LEVEL_ERROR, "CertificateInstallationRes timeout occured");
            conn->ctx->intl_emergency_shutdown = true; // [V2G2-918] Initiating emergency shutdown, response code faild
                                                       // will be set in iso_validate_response_code() function
        }
        pthread_mutex_unlock(&conn->ctx->mqtt_lock);
    }

    if ((conn->ctx->evse_v2g_data.cert_install_res_b64_buffer.empty() == false) &&
        (conn->ctx->evse_v2g_data.cert_install_status == true)) {
        if ((rv = mbedtls_base64_decode(
                 conn->buffer + V2GTP_HEADER_LENGTH, DEFAULT_BUFFER_SIZE, &conn->buffer_pos,
                 reinterpret_cast<unsigned char*>(conn->ctx->evse_v2g_data.cert_install_res_b64_buffer.data()),
                 conn->ctx->evse_v2g_data.cert_install_res_b64_buffer.size())) != 0) {
            char strerr[256];
            mbedtls_strerror(rv, strerr, 256);
            dlog(DLOG_LEVEL_ERROR, "Failed to decode base64 stream (-0x%04x) %s", rv, strerr);
            goto exit;
        }
        nextEvent = V2G_EVENT_SEND_RECV_EXI_MSG;
        res->ResponseCode = iso1responseCodeType_OK; // Is irrelevant but must be valid to serve the internal validation
        conn->buffer_pos +=
            V2GTP_HEADER_LENGTH; // buffer_pos had only the payload, so increase it to be header + payload
    } else {
        res->ResponseCode = iso1responseCodeType_FAILED;
    }

exit:
    if (V2G_EVENT_SEND_RECV_EXI_MSG != nextEvent) {
        /* Check the current response code and check if no external error has occurred */
        nextEvent = (enum v2g_event)iso_validate_response_code(&res->ResponseCode, conn);
    } else {
        /* Reset v2g-msg If there, in case of an error */
        init_iso1CertificateInstallationResType(res);
    }

    /* Set next expected req msg */
    conn->ctx->state = (int)iso_dc_state_id::WAIT_FOR_PAYMENTDETAILS; // [V2G-554]

    return nextEvent;
}

/*!
 * \brief handle_iso_cable_check This function handles the iso_cable_check msg pair. It analyzes the request msg and
 * fills the response msg. The request and response msg based on the open V2G structures. This structures must be
 * provided within the \c conn structure.
 * \param conn holds the structure with the V2G msg pair.
 * \return Returns the next V2G-event.
 */
static enum v2g_event handle_iso_cable_check(struct v2g_connection* conn) {
    struct iso1CableCheckReqType* req = &conn->exi_in.iso1EXIDocument->V2G_Message.Body.CableCheckReq;
    struct iso1CableCheckResType* res = &conn->exi_out.iso1EXIDocument->V2G_Message.Body.CableCheckRes;
    enum v2g_event next_event = V2G_EVENT_NO_EVENT;

    /* At first, publish the received EV request message to the MQTT interface */
    publish_DC_EVStatusType(conn->ctx, req->DC_EVStatus);

    // TODO: For DC charging wait for CP state C or D , before transmitting of the response ([V2G2-917], [V2G2-918]). CP
    // state is checked by other module

    /* Fill the CableCheckRes */
    res->ResponseCode = iso1responseCodeType_OK;
    res->DC_EVSEStatus.EVSEIsolationStatus = (iso1isolationLevelType)conn->ctx->evse_v2g_data.evse_isolation_status;
    res->DC_EVSEStatus.EVSEIsolationStatus_isUsed = conn->ctx->evse_v2g_data.evse_isolation_status_is_used;
    res->DC_EVSEStatus.EVSENotification = (iso1EVSENotificationType)conn->ctx->evse_v2g_data.evse_notification;
    res->DC_EVSEStatus.EVSEStatusCode =
        (iso1DC_EVSEStatusCodeType)conn->ctx->evse_v2g_data.evse_status_code[PHASE_ISOLATION];
    res->DC_EVSEStatus.NotificationMaxDelay = (uint16_t)conn->ctx->evse_v2g_data.notification_max_delay;
    res->EVSEProcessing = (iso1EVSEProcessingType)conn->ctx->evse_v2g_data.evse_processing[PHASE_ISOLATION];

    /* Check the current response code and check if no external error has occurred */
    next_event = (v2g_event)iso_validate_response_code(&res->ResponseCode, conn);

    /* Set next expected req msg */
    conn->ctx->state = (res->EVSEProcessing == iso1EVSEProcessingType_Finished)
                           ? (int)iso_dc_state_id::WAIT_FOR_PRECHARGE
                           : (int)iso_dc_state_id::WAIT_FOR_CABLECHECK; // [V2G-584], [V2G-621]

    return next_event;
}

/*!
 * \brief handle_iso_pre_charge This function handles the iso_pre_charge msg pair. It analyzes the request msg and fills
 * the response msg. The request and response msg based on the open V2G structures. This structures must be provided
 * within the \c conn structure.
 * \param conn holds the structure with the V2G msg pair.
 * \return Returns the next V2G-event.
 */
static enum v2g_event handle_iso_pre_charge(struct v2g_connection* conn) {
    struct iso1PreChargeReqType* req = &conn->exi_in.iso1EXIDocument->V2G_Message.Body.PreChargeReq;
    struct iso1PreChargeResType* res = &conn->exi_out.iso1EXIDocument->V2G_Message.Body.PreChargeRes;
    enum v2g_event next_event = V2G_EVENT_NO_EVENT;

    /* At first, publish the received EV request message to the MQTT interface */
    publish_iso_pre_charge_req(conn->ctx, req);

    /* Fill the PreChargeRes*/
    res->DC_EVSEStatus.EVSEIsolationStatus = (iso1isolationLevelType)conn->ctx->evse_v2g_data.evse_isolation_status;
    res->DC_EVSEStatus.EVSEIsolationStatus_isUsed = conn->ctx->evse_v2g_data.evse_isolation_status_is_used;
    res->DC_EVSEStatus.EVSENotification = (iso1EVSENotificationType)conn->ctx->evse_v2g_data.evse_notification;
    res->DC_EVSEStatus.EVSEStatusCode =
        (iso1DC_EVSEStatusCodeType)conn->ctx->evse_v2g_data.evse_status_code[PHASE_PRECHARGE];
    res->DC_EVSEStatus.NotificationMaxDelay = (uint16_t)conn->ctx->evse_v2g_data.notification_max_delay;
    res->EVSEPresentVoltage = (iso1PhysicalValueType)conn->ctx->evse_v2g_data.evse_present_voltage;
    res->ResponseCode = iso1responseCodeType_OK;

    /* Check the current response code and check if no external error has occurred */
    next_event = (v2g_event)iso_validate_response_code(&res->ResponseCode, conn);

    /* Set next expected req msg */
    conn->ctx->state = (int)iso_dc_state_id::WAIT_FOR_PRECHARGE_POWERDELIVERY; // [V2G-587]

    return next_event;
}

/*!
 * \brief handle_iso_current_demand This function handles the iso_current_demand msg pair. It analyzes the request msg
 * and fills the response msg. The request and response msg based on the open V2G structures. This structures must be
 * provided within the \c conn structure.
 * \param conn holds the structure with the V2G msg pair.
 * \return Returns the next V2G-event.
 */
static enum v2g_event handle_iso_current_demand(struct v2g_connection* conn) {
    struct iso1CurrentDemandReqType* req = &conn->exi_in.iso1EXIDocument->V2G_Message.Body.CurrentDemandReq;
    struct iso1CurrentDemandResType* res = &conn->exi_out.iso1EXIDocument->V2G_Message.Body.CurrentDemandRes;
    enum v2g_event next_event = V2G_EVENT_NO_EVENT;

    /* At first, publish the received EV request message to the MQTT interface */
    publish_iso_current_demand_req(conn->ctx, req);

    res->DC_EVSEStatus.EVSEIsolationStatus = (iso1isolationLevelType)conn->ctx->evse_v2g_data.evse_isolation_status;
    res->DC_EVSEStatus.EVSEIsolationStatus_isUsed = conn->ctx->evse_v2g_data.evse_isolation_status_is_used;
    res->DC_EVSEStatus.EVSENotification = (iso1EVSENotificationType)conn->ctx->evse_v2g_data.evse_notification;
    res->DC_EVSEStatus.EVSEStatusCode =
        (iso1DC_EVSEStatusCodeType)conn->ctx->evse_v2g_data.evse_status_code[PHASE_CHARGE];
    res->DC_EVSEStatus.NotificationMaxDelay = (uint16_t)conn->ctx->evse_v2g_data.notification_max_delay;
    if ((conn->ctx->evse_v2g_data.evse_maximum_current_limit_is_used == 1) &&
        (calc_physical_value(req->EVTargetCurrent.Value, req->EVTargetCurrent.Multiplier) >=
         calc_physical_value(conn->ctx->evse_v2g_data.evse_maximum_current_limit.Value,
                             conn->ctx->evse_v2g_data.evse_maximum_current_limit.Multiplier))) {
        conn->ctx->evse_v2g_data.evse_current_limit_achieved = (int)1;
    } else {
        conn->ctx->evse_v2g_data.evse_current_limit_achieved = (int)0;
    }
    res->EVSECurrentLimitAchieved = conn->ctx->evse_v2g_data.evse_current_limit_achieved;
    memcpy(res->EVSEID.characters, conn->ctx->evse_v2g_data.evse_id.bytes, conn->ctx->evse_v2g_data.evse_id.bytesLen);
    res->EVSEID.charactersLen = conn->ctx->evse_v2g_data.evse_id.bytesLen;
    res->EVSEMaximumCurrentLimit = conn->ctx->evse_v2g_data.evse_maximum_current_limit;
    res->EVSEMaximumCurrentLimit_isUsed = conn->ctx->evse_v2g_data.evse_maximum_current_limit_is_used;
    res->EVSEMaximumPowerLimit = conn->ctx->evse_v2g_data.evse_maximum_power_limit;
    res->EVSEMaximumPowerLimit_isUsed = conn->ctx->evse_v2g_data.evse_maximum_power_limit_is_used;
    res->EVSEMaximumVoltageLimit = conn->ctx->evse_v2g_data.evse_maximum_voltage_limit;
    res->EVSEMaximumVoltageLimit_isUsed = conn->ctx->evse_v2g_data.evse_maximum_voltage_limit_is_used;
    double EVTargetPower = calc_physical_value(req->EVTargetCurrent.Value, req->EVTargetCurrent.Multiplier) *
                           calc_physical_value(req->EVTargetVoltage.Value, req->EVTargetVoltage.Multiplier);
    if ((conn->ctx->evse_v2g_data.evse_maximum_power_limit_is_used == 1) &&
        (EVTargetPower >= calc_physical_value(conn->ctx->evse_v2g_data.evse_maximum_power_limit.Value,
                                              conn->ctx->evse_v2g_data.evse_maximum_power_limit.Multiplier))) {
        conn->ctx->evse_v2g_data.evse_power_limit_achieved = (int)1;
    } else {
        conn->ctx->evse_v2g_data.evse_power_limit_achieved = (int)0;
    }
    res->EVSEPowerLimitAchieved = conn->ctx->evse_v2g_data.evse_power_limit_achieved;
    res->EVSEPresentCurrent = conn->ctx->evse_v2g_data.evse_present_current;
    res->EVSEPresentVoltage = conn->ctx->evse_v2g_data.evse_present_voltage;
    if ((conn->ctx->evse_v2g_data.evse_maximum_voltage_limit_is_used == 1) &&
        (calc_physical_value(req->EVTargetVoltage.Value, req->EVTargetVoltage.Multiplier) >=
         calc_physical_value(conn->ctx->evse_v2g_data.evse_maximum_voltage_limit.Value,
                             conn->ctx->evse_v2g_data.evse_maximum_voltage_limit.Multiplier))) {
        conn->ctx->evse_v2g_data.evse_voltage_limit_achieved = (int)1;
    } else {
        conn->ctx->evse_v2g_data.evse_voltage_limit_achieved = (int)0;
    }
    res->EVSEVoltageLimitAchieved = conn->ctx->evse_v2g_data.evse_voltage_limit_achieved;
    if (conn->ctx->meter_info.meter_info_is_used == true) {
        res->MeterInfo.MeterID.charactersLen = conn->ctx->meter_info.meter_id.bytesLen;
        memcpy(res->MeterInfo.MeterID.characters, conn->ctx->meter_info.meter_id.bytes,
               iso1MeterInfoType_MeterID_CHARACTERS_SIZE);
        res->MeterInfo.MeterReading = conn->ctx->meter_info.meter_reading;
        res->MeterInfo.MeterReading_isUsed = 1;
        res->MeterInfo_isUsed = 1;
        // Reset the signal for the next time handle_set_MeterInfo is signaled
        conn->ctx->meter_info.meter_info_is_used = false;
    } else {
        res->MeterInfo_isUsed = 0;
    }
    res->ReceiptRequired = conn->ctx->evse_v2g_data.receipt_required; // TODO: PNC only
    res->ReceiptRequired_isUsed = (conn->ctx->session.iso_selected_payment_option == iso1paymentOptionType_Contract)
                                      ? (unsigned int)conn->ctx->evse_v2g_data.receipt_required
                                      : (unsigned int)0;
    res->ResponseCode = iso1responseCodeType_OK;
    res->SAScheduleTupleID = conn->ctx->session.sa_schedule_tuple_id;

    static uint8_t req_pos_value_count = 0;

    if (conn->ctx->evse_v2g_data.sae_bidi_data.enabled_sae_v2g == true) {

        // case: evse initiated -> Negative PresentCurrent, EvseMaxCurrentLimit, EvseMaxCurrentLimit
        if (conn->ctx->evse_v2g_data.sae_bidi_data.discharging == false &&
            conn->ctx->evse_v2g_data.evse_present_current.Value < 0 &&
            conn->ctx->evse_v2g_data.evse_maximum_current_limit_is_used == true &&
            conn->ctx->evse_v2g_data.evse_maximum_current_limit.Value < 0 &&
            conn->ctx->evse_v2g_data.evse_maximum_power_limit_is_used == true &&
            conn->ctx->evse_v2g_data.evse_maximum_power_limit.Value < 0) {
            if (req->EVTargetCurrent.Value > 0) {
                if (req_pos_value_count++ >= 1) {
                    dlog(DLOG_LEVEL_WARNING, "SAE V2G Bidi handshake was not recognized by the ev side. Instead of "
                                             "shutting down, it is better to wait for a correct response");
                    req_pos_value_count = 0;
                } else {
                    req_pos_value_count = 0;
                    conn->ctx->evse_v2g_data.sae_bidi_data.discharging = true;
                }
            }
        } else if (conn->ctx->evse_v2g_data.sae_bidi_data.discharging == true &&
                   conn->ctx->evse_v2g_data.evse_present_current.Value > 0 &&
                   conn->ctx->evse_v2g_data.evse_maximum_current_limit_is_used == true &&
                   conn->ctx->evse_v2g_data.evse_maximum_current_limit.Value > 0 &&
                   conn->ctx->evse_v2g_data.evse_maximum_power_limit_is_used == true &&
                   conn->ctx->evse_v2g_data.evse_maximum_power_limit.Value > 0) {
            if (req->EVTargetCurrent.Value < 0) {
                if (req_pos_value_count++ >= 1) {
                    dlog(DLOG_LEVEL_WARNING, "SAE V2G Bidi handshake was not recognized by the ev side. Instead of "
                                             "shutting down, it is better to wait for a correct response");
                    req_pos_value_count = 0;
                } else {
                    req_pos_value_count = 0;
                    conn->ctx->evse_v2g_data.sae_bidi_data.discharging = false;
                }
            }
        }

        // case: ev initiated -> Negative EvTargetCurrent, EVMaxCurrentLimit, EVMaxPowerLimit
        // Todo(SL): Is it necessary to notify the evse_manager that the ev want to give power/current?
        // Or is it obvious because of the negative target current request.

    } else if (conn->ctx->evse_v2g_data.sae_bidi_data.enabled_sae_v2h == true) {
        if (req->DC_EVStatus.EVRESSSOC <= conn->ctx->evse_v2g_data.sae_bidi_data.sae_v2h_minimal_soc) {
            res->DC_EVSEStatus.EVSEStatusCode = iso1DC_EVSEStatusCodeType_EVSE_Shutdown;
        }
    }

    /* Check the current response code and check if no external error has occurred */
    next_event = (v2g_event)iso_validate_response_code(&res->ResponseCode, conn);

    /* Set next expected req msg */
    conn->ctx->state = ((res->ReceiptRequired_isUsed == (unsigned int)1) && (res->ReceiptRequired == (int)1))
                           ? (int)iso_dc_state_id::WAIT_FOR_METERINGRECEIPT
                           : (int)iso_dc_state_id::WAIT_FOR_CURRENTDEMAND_POWERDELIVERY; // [V2G-795], [V2G-593]

    return next_event;
}

/*!
 * \brief handle_iso_welding_detection This function handles the iso_welding_detection msg pair. It analyzes the request
 * msg and fills the response msg. The request and response msg based on the open V2G structures. This structures must
 * be provided within the \c conn structure.
 * \param conn holds the structure with the V2G msg pair.
 * \return Returns the next V2G-event.
 */
static enum v2g_event handle_iso_welding_detection(struct v2g_connection* conn) {
    struct iso1WeldingDetectionReqType* req = &conn->exi_in.iso1EXIDocument->V2G_Message.Body.WeldingDetectionReq;
    struct iso1WeldingDetectionResType* res = &conn->exi_out.iso1EXIDocument->V2G_Message.Body.WeldingDetectionRes;
    enum v2g_event next_event = V2G_EVENT_NO_EVENT;

    /* At first, publish the received EV request message to the MQTT interface */
    publish_iso_welding_detection_req(conn->ctx, req);

    // TODO: Wait for CP state B, before transmitting of the response, or signal intl_emergency_shutdown in conn->ctx
    // ([V2G2-920], [V2G2-921]).

    res->DC_EVSEStatus.EVSEIsolationStatus = (iso1isolationLevelType)conn->ctx->evse_v2g_data.evse_isolation_status;
    res->DC_EVSEStatus.EVSEIsolationStatus_isUsed = conn->ctx->evse_v2g_data.evse_isolation_status_is_used;
    res->DC_EVSEStatus.EVSENotification = (iso1EVSENotificationType)conn->ctx->evse_v2g_data.evse_notification;
    res->DC_EVSEStatus.EVSEStatusCode =
        (iso1DC_EVSEStatusCodeType)conn->ctx->evse_v2g_data.evse_status_code[PHASE_WELDING];
    res->DC_EVSEStatus.NotificationMaxDelay = (uint16_t)conn->ctx->evse_v2g_data.notification_max_delay;
    res->EVSEPresentVoltage = conn->ctx->evse_v2g_data.evse_present_voltage;
    res->ResponseCode = iso1responseCodeType_OK;

    /* Check the current response code and check if no external error has occurred */
    next_event = (v2g_event)iso_validate_response_code(&res->ResponseCode, conn);

    /* Set next expected req msg */
    conn->ctx->state = (int)iso_dc_state_id::WAIT_FOR_WELDINGDETECTION_SESSIONSTOP; // [V2G-597]

    return next_event;
}

/*!
 * \brief handle_iso_session_stop This function handles the iso_session_stop msg pair. It analyses the request msg and
 * fills the response msg. The request and response msg based on the open V2G structures. This structures must be
 * provided within the \c conn structure.
 * \param conn holds the structure with the V2G msg pair.
 * \return Returns the next V2G-event.
 */
static enum v2g_event handle_iso_session_stop(struct v2g_connection* conn) {
    struct iso1SessionStopReqType* req = &conn->exi_in.iso1EXIDocument->V2G_Message.Body.SessionStopReq;
    struct iso1SessionStopResType* res = &conn->exi_out.iso1EXIDocument->V2G_Message.Body.SessionStopRes;

    res->ResponseCode = iso1responseCodeType_OK;

    /* Check the current response code and check if no external error has occurred */
    iso_validate_response_code(&res->ResponseCode, conn);

    /* Set the next charging state */
    switch (req->ChargingSession) {
    case iso1chargingSessionType_Terminate:
        conn->dlink_action = MQTT_DLINK_ACTION_TERMINATE;
        conn->ctx->hlc_pause_active = false;
        /* Set next expected req msg */
        conn->ctx->state = (int)iso_dc_state_id::WAIT_FOR_TERMINATED_SESSION;
        break;

    case iso1chargingSessionType_Pause:
        /* Set next expected req msg */
        /* Check if the EV is allowed to request the sleep mode. TODO: Remove "true" if sleep mode is supported */
        if (((conn->ctx->last_v2g_msg != V2G_POWER_DELIVERY_MSG) &&
             (conn->ctx->last_v2g_msg != V2G_WELDING_DETECTION_MSG))) {
            conn->dlink_action = MQTT_DLINK_ACTION_TERMINATE;
            res->ResponseCode = iso1responseCodeType_FAILED;
            conn->ctx->hlc_pause_active = false;
            conn->ctx->state = (int)iso_dc_state_id::WAIT_FOR_TERMINATED_SESSION;
        } else {
            /* Init sleep mode for the EV */
            conn->dlink_action = MQTT_DLINK_ACTION_PAUSE;
            conn->ctx->hlc_pause_active = true;
            conn->ctx->state = (int)iso_dc_state_id::WAIT_FOR_SESSIONSETUP;
        }
        break;

    default:
        /* Set next expected req msg */
        conn->dlink_action = MQTT_DLINK_ACTION_TERMINATE;
        conn->ctx->state = (int)iso_dc_state_id::WAIT_FOR_TERMINATED_SESSION;
    }

    return V2G_EVENT_SEND_AND_TERMINATE; // Charging must be terminated after sending the response message [V2G2-571]
}

enum v2g_event iso_handle_request(v2g_connection* conn) {
    struct iso1EXIDocument* exi_in = conn->exi_in.iso1EXIDocument;
    struct iso1EXIDocument* exi_out = conn->exi_out.iso1EXIDocument;
    bool resume_trial;
    enum v2g_event next_v2g_event = V2G_EVENT_TERMINATE_CONNECTION;

    /* check whether we have a valid EXI document embedded within a V2G message */
    if (!exi_in->V2G_Message_isUsed) {
        dlog(DLOG_LEVEL_ERROR, "V2G_Message not used");
        return V2G_EVENT_IGNORE_MSG;
    }

    /* extract session id */
    conn->ctx->ev_v2g_data.received_session_id = v2g_session_id_from_exi(true, exi_in);

    /* init V2G structure (document, header, body) */
    init_iso1EXIDocument(exi_out);
    exi_out->V2G_Message_isUsed = 1u;
    init_iso1MessageHeaderType(&exi_out->V2G_Message.Header);

    exi_out->V2G_Message.Header.SessionID.bytesLen = 8;
    init_iso1BodyType(&exi_out->V2G_Message.Body);

    /* handle each message type individually;
     * we use a none-usual source code formatting here to optically group the individual
     * request a little bit
     */
    if (exi_in->V2G_Message.Body.CurrentDemandReq_isUsed) {
        dlog(DLOG_LEVEL_TRACE, "Handling CurrentDemandReq");
        if (conn->ctx->last_v2g_msg == V2G_POWER_DELIVERY_MSG) {
            conn->ctx->p_charger->publish_currentDemand_Started(nullptr);
            conn->ctx->session.is_charging = true;
        }
        conn->ctx->current_v2g_msg = V2G_CURRENT_DEMAND_MSG;
        exi_out->V2G_Message.Body.CurrentDemandRes_isUsed = 1u;
        init_iso1CurrentDemandResType(&exi_out->V2G_Message.Body.CurrentDemandRes);
        next_v2g_event = handle_iso_current_demand(conn); //  [V2G2-592]
    } else if (exi_in->V2G_Message.Body.SessionSetupReq_isUsed) {
        dlog(DLOG_LEVEL_TRACE, "Handling SessionSetupReq");
        conn->ctx->current_v2g_msg = V2G_SESSION_SETUP_MSG;
        exi_out->V2G_Message.Body.SessionSetupRes_isUsed = 1u;
        init_iso1SessionSetupResType(&exi_out->V2G_Message.Body.SessionSetupRes);
        next_v2g_event = handle_iso_session_setup(conn); // [V2G2-542]
    } else if (exi_in->V2G_Message.Body.ServiceDiscoveryReq_isUsed) {
        dlog(DLOG_LEVEL_TRACE, "Handling ServiceDiscoveryReq");
        conn->ctx->current_v2g_msg = V2G_SERVICE_DISCOVERY_MSG;
        exi_out->V2G_Message.Body.ServiceDiscoveryRes_isUsed = 1u;
        init_iso1ServiceDiscoveryResType(&exi_out->V2G_Message.Body.ServiceDiscoveryRes);
        next_v2g_event = handle_iso_service_discovery(conn); // [V2G2-542]
    } else if (exi_in->V2G_Message.Body.ServiceDetailReq_isUsed) {
        dlog(DLOG_LEVEL_TRACE, "Handling ServiceDetailReq");
        conn->ctx->current_v2g_msg = V2G_SERVICE_DETAIL_MSG;
        exi_out->V2G_Message.Body.ServiceDetailRes_isUsed = 1u;
        init_iso1ServiceDetailResType(&exi_out->V2G_Message.Body.ServiceDetailRes);
        next_v2g_event = handle_iso_service_detail(conn); // [V2G2-547]
    } else if (exi_in->V2G_Message.Body.PaymentServiceSelectionReq_isUsed) {
        dlog(DLOG_LEVEL_TRACE, "Handling PaymentServiceSelectionReq");
        conn->ctx->current_v2g_msg = V2G_PAYMENT_SERVICE_SELECTION_MSG;
        exi_out->V2G_Message.Body.PaymentServiceSelectionRes_isUsed = 1u;
        init_iso1PaymentServiceSelectionResType(&exi_out->V2G_Message.Body.PaymentServiceSelectionRes);
        next_v2g_event = handle_iso_payment_service_selection(conn); // [V2G2-550]
    } else if (exi_in->V2G_Message.Body.PaymentDetailsReq_isUsed) {
        dlog(DLOG_LEVEL_TRACE, "Handling PaymentDetailsReq");
        conn->ctx->current_v2g_msg = V2G_PAYMENT_DETAILS_MSG;
        exi_out->V2G_Message.Body.PaymentDetailsRes_isUsed = 1u;
        init_iso1PaymentDetailsResType(&exi_out->V2G_Message.Body.PaymentDetailsRes);
        next_v2g_event = handle_iso_payment_details(conn); // [V2G2-559]
    } else if (exi_in->V2G_Message.Body.AuthorizationReq_isUsed) {
        dlog(DLOG_LEVEL_TRACE, "Handling AuthorizationReq");
        conn->ctx->current_v2g_msg = V2G_AUTHORIZATION_MSG;
        if (conn->ctx->last_v2g_msg != V2G_AUTHORIZATION_MSG) {
            if (conn->ctx->session.iso_selected_payment_option == iso1paymentOptionType_ExternalPayment) {
                conn->ctx->p_charger->publish_Require_Auth_EIM(nullptr);
            }
        }
        exi_out->V2G_Message.Body.AuthorizationRes_isUsed = 1u;
        init_iso1AuthorizationResType(&exi_out->V2G_Message.Body.AuthorizationRes);
        next_v2g_event = handle_iso_authorization(conn); // [V2G2-562]
    } else if (exi_in->V2G_Message.Body.ChargeParameterDiscoveryReq_isUsed) {
        dlog(DLOG_LEVEL_TRACE, "Handling ChargeParameterDiscoveryReq");
        conn->ctx->current_v2g_msg = V2G_CHARGE_PARAMETER_DISCOVERY_MSG;
        if (conn->ctx->last_v2g_msg == V2G_AUTHORIZATION_MSG) {
            dlog(DLOG_LEVEL_INFO, "Parameter-phase started");
        }
        exi_out->V2G_Message.Body.ChargeParameterDiscoveryRes_isUsed = 1u;
        init_iso1ChargeParameterDiscoveryResType(&exi_out->V2G_Message.Body.ChargeParameterDiscoveryRes);
        next_v2g_event = handle_iso_charge_parameter_discovery(conn); // [V2G2-565]
    } else if (exi_in->V2G_Message.Body.PowerDeliveryReq_isUsed) {
        dlog(DLOG_LEVEL_TRACE, "Handling PowerDeliveryReq");
        conn->ctx->current_v2g_msg = V2G_POWER_DELIVERY_MSG;
        exi_out->V2G_Message.Body.PowerDeliveryRes_isUsed = 1u;
        init_iso1PowerDeliveryResType(&exi_out->V2G_Message.Body.PowerDeliveryRes);
        next_v2g_event = handle_iso_power_delivery(conn); // [V2G2-589]
    } else if (exi_in->V2G_Message.Body.ChargingStatusReq_isUsed) {
        dlog(DLOG_LEVEL_TRACE, "Handling ChargingStatusReq");
        conn->ctx->current_v2g_msg = V2G_CHARGING_STATUS_MSG;

        exi_out->V2G_Message.Body.ChargingStatusRes_isUsed = 1u;
        init_iso1ChargingStatusResType(&exi_out->V2G_Message.Body.ChargingStatusRes);
        next_v2g_event = handle_iso_charging_status(conn);
    } else if (exi_in->V2G_Message.Body.MeteringReceiptReq_isUsed) {
        dlog(DLOG_LEVEL_TRACE, "Handling MeteringReceiptReq");
        conn->ctx->current_v2g_msg = V2G_METERING_RECEIPT_MSG;
        exi_out->V2G_Message.Body.MeteringReceiptRes_isUsed = 1u;
        init_iso1MeteringReceiptResType(&exi_out->V2G_Message.Body.MeteringReceiptRes);
        next_v2g_event = handle_iso_metering_receipt(conn); // [V2G2-796]
    } else if (exi_in->V2G_Message.Body.CertificateUpdateReq_isUsed) {
        dlog(DLOG_LEVEL_TRACE, "Handling CertificateUpdateReq");
        conn->ctx->current_v2g_msg = V2G_CERTIFICATE_UPDATE_MSG;

        exi_out->V2G_Message.Body.CertificateUpdateRes_isUsed = 1u;
        init_iso1CertificateUpdateResType(&exi_out->V2G_Message.Body.CertificateUpdateRes);
        next_v2g_event = handle_iso_certificate_update(conn); // [V2G2-556]
    } else if (exi_in->V2G_Message.Body.CertificateInstallationReq_isUsed) {
        dlog(DLOG_LEVEL_TRACE, "Handling CertificateInstallationReq");
        conn->ctx->current_v2g_msg = V2G_CERTIFICATE_INSTALLATION_MSG;
        dlog(DLOG_LEVEL_INFO, "CertificateInstallation-phase started");

        exi_out->V2G_Message.Body.CertificateInstallationRes_isUsed = 1u;
        init_iso1CertificateInstallationResType(&exi_out->V2G_Message.Body.CertificateInstallationRes);
        next_v2g_event = handle_iso_certificate_installation(conn); // [V2G2-553]
    } else if (exi_in->V2G_Message.Body.CableCheckReq_isUsed) {
        dlog(DLOG_LEVEL_TRACE, "Handling CableCheckReq");
        conn->ctx->current_v2g_msg = V2G_CABLE_CHECK_MSG;
        /* At first send mqtt charging phase signal to the customer interface */
        if (V2G_CHARGE_PARAMETER_DISCOVERY_MSG == conn->ctx->last_v2g_msg) {
            conn->ctx->p_charger->publish_Start_CableCheck(nullptr);
        }

        exi_out->V2G_Message.Body.CableCheckRes_isUsed = 1u;
        init_iso1CableCheckResType(&exi_out->V2G_Message.Body.CableCheckRes);
        next_v2g_event = handle_iso_cable_check(conn); // [V2G2-583
    } else if (exi_in->V2G_Message.Body.PreChargeReq_isUsed) {
        dlog(DLOG_LEVEL_TRACE, "Handling PreChargeReq");
        conn->ctx->current_v2g_msg = V2G_PRE_CHARGE_MSG;
        /* At first send  mqtt charging phase signal to the customer interface */
        if (conn->ctx->last_v2g_msg == V2G_CABLE_CHECK_MSG) {
            dlog(DLOG_LEVEL_INFO, "Precharge-phase started");
        }

        exi_out->V2G_Message.Body.PreChargeRes_isUsed = 1u;
        init_iso1PreChargeResType(&exi_out->V2G_Message.Body.PreChargeRes);
        next_v2g_event = handle_iso_pre_charge(conn); // [V2G2-586]
    } else if (exi_in->V2G_Message.Body.WeldingDetectionReq_isUsed) {
        dlog(DLOG_LEVEL_TRACE, "Handling WeldingDetectionReq");
        conn->ctx->current_v2g_msg = V2G_WELDING_DETECTION_MSG;
        if (conn->ctx->last_v2g_msg != V2G_WELDING_DETECTION_MSG) {
            dlog(DLOG_LEVEL_INFO, "Welding-phase started");
        }
        exi_out->V2G_Message.Body.WeldingDetectionRes_isUsed = 1u;
        init_iso1WeldingDetectionResType(&exi_out->V2G_Message.Body.WeldingDetectionRes);
        next_v2g_event = handle_iso_welding_detection(conn); // [V2G2-596]
    } else if (exi_in->V2G_Message.Body.SessionStopReq_isUsed) {
        dlog(DLOG_LEVEL_TRACE, "Handling SessionStopReq");
        conn->ctx->current_v2g_msg = V2G_SESSION_STOP_MSG;
        exi_out->V2G_Message.Body.SessionStopRes_isUsed = 1u;
        init_iso1SessionStopResType(&exi_out->V2G_Message.Body.SessionStopRes);
        next_v2g_event = handle_iso_session_stop(conn); // [V2G2-570]
    } else {
        dlog(DLOG_LEVEL_ERROR, "create_response_message: request type not found");
        next_v2g_event = V2G_EVENT_IGNORE_MSG;
    }
    dlog(DLOG_LEVEL_TRACE, "Current state: %s",
         conn->ctx->is_dc_charger ? iso_dc_states[conn->ctx->state].description
                                  : iso_ac_states[conn->ctx->state].description);

    // If next_v2g_event == V2G_EVENT_IGNORE_MSG, keep the current state and ignore msg
    if (next_v2g_event != V2G_EVENT_IGNORE_MSG) {
        conn->ctx->last_v2g_msg = conn->ctx->current_v2g_msg;

        /* Configure session id */
        memcpy(exi_out->V2G_Message.Header.SessionID.bytes, &conn->ctx->evse_v2g_data.session_id,
               iso1MessageHeaderType_SessionID_BYTES_SIZE);

        /* We always set bytesLen to iso1MessageHeaderType_SessionID_BYTES_SIZE */
        exi_out->V2G_Message.Header.SessionID.bytesLen = iso1MessageHeaderType_SessionID_BYTES_SIZE;
    }

    return next_v2g_event;
}
