// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ocpp/v2/functional_blocks/authorization.hpp>

#include <ocpp/common/constants.hpp>
#include <ocpp/common/evse_security.hpp>
#include <ocpp/v2/connectivity_manager.hpp>
#include <ocpp/v2/ctrlr_component_variables.hpp>
#include <ocpp/v2/database_handler.hpp>
#include <ocpp/v2/device_model.hpp>
#include <ocpp/v2/functional_blocks/functional_block_context.hpp>
#include <ocpp/v2/utils.hpp>

#include <ocpp/v2/messages/Authorize.hpp>
#include <ocpp/v2/messages/ClearCache.hpp>
#include <ocpp/v2/messages/GetLocalListVersion.hpp>
#include <ocpp/v2/messages/SendLocalList.hpp>

///
/// \brief Check if vector of authorization data has a duplicate id token.
/// \param list List to check.
/// \return True if there is a duplicate.
///
static bool has_duplicate_in_list(const std::vector<ocpp::v2::AuthorizationData>& list);
static bool has_no_token_info(const ocpp::v2::AuthorizationData& item);

ocpp::v2::Authorization::Authorization(const FunctionalBlockContext& context) :
    context(context), auth_cache_cleanup_handler_running(false) {
}

ocpp::v2::Authorization::~Authorization() {
    stop_auth_cache_cleanup_thread();
}

void ocpp::v2::Authorization::start_auth_cache_cleanup_thread() {
    if (!auth_cache_cleanup_handler_running) {
        auth_cache_cleanup_handler_running = true;
        this->auth_cache_cleanup_thread = std::thread(&Authorization::cache_cleanup_handler, this);
    }
}

void ocpp::v2::Authorization::handle_message(const ocpp::EnhancedMessage<MessageType>& message) {
    const auto& json_message = message.message;

    if (message.messageType == MessageType::ClearCache) {
        this->handle_clear_cache_req(json_message);
    } else if (message.messageType == MessageType::SendLocalList) {
        this->handle_send_local_authorization_list_req(json_message);
    } else if (message.messageType == MessageType::GetLocalListVersion) {
        this->handle_get_local_authorization_list_version_req(json_message);
    } else {
        throw MessageTypeNotImplementedException(message.messageType);
    }
}

ocpp::v2::AuthorizeResponse
ocpp::v2::Authorization::authorize_req(const IdToken id_token, const std::optional<ocpp::CiString<10000>>& certificate,
                                       const std::optional<std::vector<OCSPRequestData>>& ocsp_request_data) {
    AuthorizeRequest req;
    req.idToken = id_token;
    req.certificate = certificate;
    req.iso15118CertificateHashData = ocsp_request_data;

    AuthorizeResponse response;
    response.idTokenInfo.status = AuthorizationStatusEnum::Unknown;

    if (!this->context.connectivity_manager.is_websocket_connected()) {
        return response;
    }

    ocpp::Call<AuthorizeRequest> call(req);
    auto future = this->context.message_dispatcher.dispatch_call_async(call);

    if (future.wait_for(DEFAULT_WAIT_FOR_FUTURE_TIMEOUT) == std::future_status::timeout) {
        EVLOG_warning << "Waiting for DataTransfer.conf(Authorize) future timed out!";
        return response;
    }

    EnhancedMessage<MessageType> enhanced_message;
    try {
        enhanced_message = future.get();
    } catch (const EnumConversionException& e) {
        EVLOG_error << "EnumConversionException during handling of message: " << e.what();
        return response;
    }

    if (enhanced_message.messageType != MessageType::AuthorizeResponse) {
        return response;
    }

    try {
        ocpp::CallResult<AuthorizeResponse> call_result = enhanced_message.message;
        return call_result.msg;
    } catch (const EnumConversionException& e) {
        // We don't get here normally, because the future.get() already throws. Code was not removed, because something
        // might be overseen here.
        EVLOG_error << "EnumConversionException during handling of message: " << e.what();
        auto call_error = CallError(enhanced_message.uniqueId, "FormationViolation", e.what(), json({}));
        this->context.message_dispatcher.dispatch_call_error(call_error);
        return response;
    }
}

void ocpp::v2::Authorization::trigger_authorization_cache_cleanup() {
    {
        std::scoped_lock lk(this->auth_cache_cleanup_mutex);
        this->auth_cache_cleanup_required = true;
    }
    this->auth_cache_cleanup_cv.notify_one();
}

void ocpp::v2::Authorization::update_authorization_cache_size() {
    auto& auth_cache_size = ControllerComponentVariables::AuthCacheStorage;
    if (auth_cache_size.variable.has_value()) {
        try {
            auto size = this->context.database_handler.authorization_cache_get_binary_size();
            this->context.device_model.set_read_only_value(auth_cache_size.component, auth_cache_size.variable.value(),
                                                           AttributeEnum::Actual, std::to_string(size),
                                                           VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL);
        } catch (const common::DatabaseException& e) {
            EVLOG_warning << "Could not get authorization cache binary size from database: " << e.what();
        } catch (const std::exception& e) {
            EVLOG_warning << "Could not get authorization cache binary size from database" << e.what();
        }
    }
}

bool ocpp::v2::Authorization::is_auth_cache_ctrlr_enabled() {
    return this->context.device_model.get_optional_value<bool>(ControllerComponentVariables::AuthCacheCtrlrEnabled)
        .value_or(false);
}

void ocpp::v2::Authorization::authorization_cache_insert_entry(const std::string& id_token_hash,
                                                               const IdTokenInfo& id_token_info) {
    this->context.database_handler.authorization_cache_insert_entry(id_token_hash, id_token_info);
}

std::optional<ocpp::v2::AuthorizationCacheEntry>
ocpp::v2::Authorization::authorization_cache_get_entry(const std::string& id_token_hash) {
    return this->context.database_handler.authorization_cache_get_entry(id_token_hash);
}

void ocpp::v2::Authorization::authorization_cache_delete_entry(const std::string& id_token_hash) {
    this->context.database_handler.authorization_cache_delete_entry(id_token_hash);
}

ocpp::v2::AuthorizeResponse
ocpp::v2::Authorization::validate_token(const IdToken id_token, const std::optional<CiString<10000>>& certificate,
                                        const std::optional<std::vector<OCSPRequestData>>& ocsp_request_data) {
    // TODO(piet): C01.FR.14
    // TODO(piet): C01.FR.15
    // TODO(piet): C01.FR.16
    // TODO(piet): C01.FR.17

    // TODO(piet): C10.FR.06

    AuthorizeResponse response;

    // C03.FR.01 && C05.FR.01: We SHALL NOT send an authorize reqeust for IdTokenType Central
    if (id_token.type == IdTokenEnumStringType::Central or
        !this->context.device_model.get_optional_value<bool>(ControllerComponentVariables::AuthCtrlrEnabled)
             .value_or(true)) {
        response.idTokenInfo.status = AuthorizationStatusEnum::Accepted;
        return response;
    }

    // C07: Authorization using contract certificates
    if (id_token.type == IdTokenEnumStringType::eMAID) {
        // Temporary variable that is set to true to avoid immediate response to allow the local auth list
        // or auth cache to be tried
        bool try_local_auth_list_or_cache = false;
        bool forwarded_to_csms = false;

        // If OCSP data is provided as argument, use it
        if (this->context.connectivity_manager.is_websocket_connected() and ocsp_request_data.has_value()) {
            EVLOG_info << "Online: Pass provided OCSP data to CSMS";
            response = this->authorize_req(id_token, std::nullopt, ocsp_request_data);
            forwarded_to_csms = true;
        } else if (certificate.has_value()) {
            // First try to validate the contract certificate locally
            CertificateValidationResult local_verify_result = this->context.evse_security.verify_certificate(
                certificate.value().get(), {ocpp::LeafCertificateType::MO, ocpp::LeafCertificateType::V2G});
            EVLOG_info << "Local contract validation result: " << local_verify_result;

            bool central_contract_validation_allowed =
                this->context.device_model
                    .get_optional_value<bool>(ControllerComponentVariables::CentralContractValidationAllowed)
                    .value_or(true);
            bool contract_validation_offline =
                this->context.device_model
                    .get_optional_value<bool>(ControllerComponentVariables::ContractValidationOffline)
                    .value_or(true);
            bool local_authorize_offline =
                this->context.device_model.get_optional_value<bool>(ControllerComponentVariables::LocalAuthorizeOffline)
                    .value_or(true);

            // C07.FR.01: When CS is online, it shall send an AuthorizeRequest
            // C07.FR.02: The AuthorizeRequest shall at least contain the OCSP data
            // TODO: local validation results are ignored if response is based only on OCSP data, is that acceptable?
            if (this->context.connectivity_manager.is_websocket_connected()) {
                // If no OCSP data was provided, check for a contract root
                if (local_verify_result == CertificateValidationResult::IssuerNotFound) {
                    // C07.FR.06: Pass contract validation to CSMS when no contract root is found
                    if (central_contract_validation_allowed) {
                        EVLOG_info << "Online: No local contract root found. Pass contract validation to CSMS";
                        response = this->authorize_req(id_token, certificate, std::nullopt);
                        forwarded_to_csms = true;
                    } else {
                        EVLOG_warning << "Online: Central Contract Validation not allowed";
                        response.idTokenInfo.status = AuthorizationStatusEnum::Invalid;
                    }
                } else {
                    // Try to generate the OCSP data from the certificate chain and use that
                    const auto generated_ocsp_request_data_list = ocpp::evse_security_conversions::to_ocpp_v2(
                        this->context.evse_security.get_mo_ocsp_request_data(certificate.value()));
                    if (generated_ocsp_request_data_list.size() > 0) {
                        EVLOG_info << "Online: Pass generated OCSP data to CSMS";
                        response = this->authorize_req(id_token, std::nullopt, generated_ocsp_request_data_list);
                        forwarded_to_csms = true;
                    } else {
                        if (central_contract_validation_allowed) {
                            EVLOG_info << "Online: OCSP data could not be generated. Pass contract validation to CSMS";
                            response = this->authorize_req(id_token, certificate, std::nullopt);
                            forwarded_to_csms = true;
                        } else {
                            EVLOG_warning
                                << "Online: OCSP data could not be generated and CentralContractValidation not allowed";
                            response.idTokenInfo.status = AuthorizationStatusEnum::Invalid;
                        }
                    }
                }
            } else { // Offline
                // C07.FR.08: CS shall try to validate the contract locally
                if (contract_validation_offline) {
                    EVLOG_info << "Offline: contract " << local_verify_result;
                    switch (local_verify_result) {
                    // C07.FR.09: CS shall lookup the eMAID in Local Auth List or Auth Cache when
                    // local validation succeeded
                    case CertificateValidationResult::Valid:
                        // In C07.FR.09 LocalAuthorizeOffline is mentioned, this seems to be a generic config item
                        // that applies to Local Auth List and Auth Cache, but since there are no requirements about
                        // it, lets check it here
                        if (local_authorize_offline) {
                            try_local_auth_list_or_cache = true;
                        } else {
                            // No requirement states what to do when ContractValidationOffline is true
                            // and LocalAuthorizeOffline is false
                            response.idTokenInfo.status = AuthorizationStatusEnum::Unknown;
                            response.certificateStatus = AuthorizeCertificateStatusEnum::Accepted;
                        }
                        break;
                    case CertificateValidationResult::Expired:
                        response.idTokenInfo.status = AuthorizationStatusEnum::Expired;
                        response.certificateStatus = AuthorizeCertificateStatusEnum::CertificateExpired;
                        break;
                    case CertificateValidationResult::InvalidSignature:
                    case CertificateValidationResult::IssuerNotFound:
                    case CertificateValidationResult::InvalidLeafSignature:
                    case CertificateValidationResult::InvalidChain:
                    case CertificateValidationResult::Unknown:
                        response.idTokenInfo.status = AuthorizationStatusEnum::Unknown;
                        break;
                    }
                } else {
                    // C07.FR.07: CS shall not allow charging
                    response.idTokenInfo.status = AuthorizationStatusEnum::NotAtThisTime;
                }
            }
        } else {
            EVLOG_warning << "Can not validate eMAID without certificate chain";
            response.idTokenInfo.status = AuthorizationStatusEnum::Invalid;
        }
        if (forwarded_to_csms) {
            // AuthorizeRequest sent to CSMS, let's show the results
            EVLOG_info << "CSMS idToken status: " << response.idTokenInfo.status;
            if (response.certificateStatus.has_value()) {
                EVLOG_info << "CSMS certificate status: " << response.certificateStatus.value();
            }
        }
        // For eMAID, we will respond here, unless the local auth list or auth cache is tried
        if (!try_local_auth_list_or_cache) {
            return response;
        }
    }

    if (this->context.device_model.get_optional_value<bool>(ControllerComponentVariables::LocalAuthListCtrlrEnabled)
            .value_or(false)) {
        std::optional<IdTokenInfo> id_token_info = std::nullopt;
        try {
            id_token_info = this->context.database_handler.get_local_authorization_list_entry(id_token);
        } catch (const common::DatabaseException& e) {
            EVLOG_warning << "Could not request local authorization list entry: " << e.what();
        } catch (const std::exception& e) {
            EVLOG_error << "Unknown Error while requesting IdTokenInfo: " << e.what();
        }

        if (id_token_info.has_value()) {
            if (id_token_info.value().status == AuthorizationStatusEnum::Accepted) {
                // C14.FR.02: If found in local list we shall start charging without an AuthorizeRequest
                EVLOG_info << "Found valid entry in local authorization list";
                response.idTokenInfo = id_token_info.value();
            } else if (this->context.device_model
                           .get_optional_value<bool>(ControllerComponentVariables::DisableRemoteAuthorization)
                           .value_or(false)) {
                EVLOG_info << "Found invalid entry in local authorization list but not sending Authorize.req because "
                              "RemoteAuthorization is disabled";
                response.idTokenInfo.status = AuthorizationStatusEnum::Unknown;
            } else if (this->context.connectivity_manager.is_websocket_connected()) {
                // C14.FR.03: If a value found but not valid we shall send an authorize request
                EVLOG_info << "Found invalid entry in local authorization list: Sending Authorize.req";
                response = this->authorize_req(id_token, certificate, ocsp_request_data);
            } else {
                // errata C13.FR.04: even in the offline state we should not authorize if present (and not accepted)
                EVLOG_info << "Found invalid entry in local authorization list whilst offline: Not authorized";
                response.idTokenInfo.status = AuthorizationStatusEnum::Unknown;
            }
            return response;
        }
    }

    const auto hashed_id_token = utils::generate_token_hash(id_token);
    const auto auth_cache_enabled = this->is_auth_cache_ctrlr_enabled();

    if (auth_cache_enabled) {
        try {
            const auto cache_entry = this->authorization_cache_get_entry(hashed_id_token);
            if (cache_entry.has_value()) {
                const auto now = DateTime();
                const IdTokenInfo& id_token_info = cache_entry->id_token_info;

                const auto lifetime =
                    this->context.device_model.get_optional_value<int>(ControllerComponentVariables::AuthCacheLifeTime);
                const bool lifetime_expired =
                    lifetime.has_value() and ((cache_entry->last_used.to_time_point() +
                                               std::chrono::seconds(lifetime.value())) < now.to_time_point());
                const bool cache_expiry_passed =
                    id_token_info.cacheExpiryDateTime.has_value() and (id_token_info.cacheExpiryDateTime.value() < now);

                if (lifetime_expired or cache_expiry_passed) {
                    EVLOG_info << "Found valid entry in AuthCache but "
                               << (lifetime_expired ? "lifetime expired" : "expiry date passed")
                               << ": Removing from cache and sending new request";
                    this->authorization_cache_delete_entry(hashed_id_token);
                    this->update_authorization_cache_size();
                } else if (this->context.device_model.get_value<bool>(
                               ControllerComponentVariables::LocalPreAuthorize) and
                           id_token_info.status == AuthorizationStatusEnum::Accepted) {
                    EVLOG_info << "Found valid entry in AuthCache";
                    this->context.database_handler.authorization_cache_update_last_used(hashed_id_token);
                    response.idTokenInfo = id_token_info;
                    return response;
                } else if (this->context.device_model
                               .get_optional_value<bool>(ControllerComponentVariables::AuthCacheDisablePostAuthorize)
                               .value_or(false)) {
                    EVLOG_info << "Found invalid entry in AuthCache: Not sending new request because "
                                  "AuthCacheDisablePostAuthorize is enabled";
                    response.idTokenInfo = id_token_info;
                    return response;
                } else {
                    EVLOG_info << "Found invalid entry in AuthCache: Sending new request";
                }
            }
        } catch (const common::DatabaseException& e) {
            EVLOG_error << "Database Error: " << e.what();
        } catch (const json::exception& e) {
            EVLOG_warning << "Could not parse data of IdTokenInfo: " << e.what();
        } catch (const std::exception& e) {
            EVLOG_error << "Unknown Error while parsing IdTokenInfo: " << e.what();
        }
    }

    if (!this->context.connectivity_manager.is_websocket_connected() and
        this->context.device_model.get_optional_value<bool>(ControllerComponentVariables::OfflineTxForUnknownIdEnabled)
            .value_or(false)) {
        EVLOG_info << "Offline authorization due to OfflineTxForUnknownIdEnabled being enabled";
        response.idTokenInfo.status = AuthorizationStatusEnum::Accepted;
        return response;
    }

    // When set to true this instructs the Charging Station to not issue any AuthorizationRequests, but only use
    // Authorization Cache and Local Authorization List to determine validity of idTokens.
    if (!this->context.device_model.get_optional_value<bool>(ControllerComponentVariables::DisableRemoteAuthorization)
             .value_or(false)) {
        response = this->authorize_req(id_token, certificate, ocsp_request_data);

        if (auth_cache_enabled) {
            try {
                this->authorization_cache_insert_entry(hashed_id_token, response.idTokenInfo);
            } catch (const common::DatabaseException& e) {
                EVLOG_error << "Could not insert into authorization cache entry: " << e.what();
            }
            this->trigger_authorization_cache_cleanup();
        }

        return response;
    }

    EVLOG_info << "Not sending Authorize.req because RemoteAuthorization is disabled";

    response.idTokenInfo.status = AuthorizationStatusEnum::Unknown;
    return response;
}

void ocpp::v2::Authorization::stop_auth_cache_cleanup_thread() {
    if (this->auth_cache_cleanup_handler_running) {
        {
            std::scoped_lock lk(this->auth_cache_cleanup_mutex);
            this->auth_cache_cleanup_handler_running = false;
        }
        this->auth_cache_cleanup_cv.notify_one();

        if (this->auth_cache_cleanup_thread.joinable()) {
            this->auth_cache_cleanup_thread.join();
        }
    }
}

void ocpp::v2::Authorization::handle_clear_cache_req(Call<ClearCacheRequest> call) {
    ClearCacheResponse response;
    response.status = ClearCacheStatusEnum::Rejected;

    if (this->is_auth_cache_ctrlr_enabled()) {
        try {
            this->context.database_handler.authorization_cache_clear();
            this->update_authorization_cache_size();
            response.status = ClearCacheStatusEnum::Accepted;
        } catch (common::DatabaseException& e) {
            auto call_error = CallError(call.uniqueId, "InternalError",
                                        "Database error while clearing authorization cache", json({}, true));
            this->context.message_dispatcher.dispatch_call_error(call_error);
            return;
        }
    }

    ocpp::CallResult<ClearCacheResponse> call_result(response, call.uniqueId);
    this->context.message_dispatcher.dispatch_call_result(call_result);
}

void ocpp::v2::Authorization::cache_cleanup_handler() {
    // Run the update once so the ram variable gets initialized
    this->update_authorization_cache_size();

    while (true) {
        {
            // Wait for next wakeup or timeout
            std::unique_lock lk(this->auth_cache_cleanup_mutex);
            if (this->auth_cache_cleanup_cv.wait_for(lk, std::chrono::minutes(15), [&]() {
                    return !this->auth_cache_cleanup_handler_running or this->auth_cache_cleanup_required;
                })) {
                EVLOG_debug << "Triggered authorization cache cleanup";
            } else {
                EVLOG_debug << "Time based authorization cache cleanup";
            }
            this->auth_cache_cleanup_required = false;
        }

        if (!this->auth_cache_cleanup_handler_running) {
            break;
        }

        auto lifetime =
            this->context.device_model.get_optional_value<int>(ControllerComponentVariables::AuthCacheLifeTime);
        try {
            this->context.database_handler.authorization_cache_delete_expired_entries(
                lifetime.has_value() ? std::optional<std::chrono::seconds>(*lifetime) : std::nullopt);

            auto meta_data = this->context.device_model.get_variable_meta_data(
                ControllerComponentVariables::AuthCacheStorage.component,
                ControllerComponentVariables::AuthCacheStorage.variable.value());

            if (meta_data.has_value()) {
                auto max_storage = meta_data->characteristics.maxLimit;
                if (max_storage.has_value()) {
                    while (this->context.database_handler.authorization_cache_get_binary_size() > max_storage.value()) {
                        this->context.database_handler.authorization_cache_delete_nr_of_oldest_entries(1);
                    }
                }
            }
        } catch (const common::DatabaseException& e) {
            EVLOG_warning << "Could not delete expired authorization cache entries from database: " << e.what();
        } catch (const std::exception& e) {
            EVLOG_warning << "Could not delete expired authorization cache entries from database: " << e.what();
        }

        this->update_authorization_cache_size();
    }
}

void ocpp::v2::Authorization::handle_send_local_authorization_list_req(Call<SendLocalListRequest> call) {
    SendLocalListResponse response;

    if (this->context.device_model.get_optional_value<bool>(ControllerComponentVariables::LocalAuthListCtrlrEnabled)
            .value_or(false)) {
        response.status = apply_local_authorization_list(call.msg);
    } else {
        response.status = SendLocalListStatusEnum::Failed;
    }

    // Set nr of entries in device_model
    if (response.status == SendLocalListStatusEnum::Accepted) {
        try {
            this->context.database_handler.insert_or_update_local_authorization_list_version(call.msg.versionNumber);
            auto& local_entries = ControllerComponentVariables::LocalAuthListCtrlrEntries;
            if (local_entries.variable.has_value()) {
                try {
                    auto entries = this->context.database_handler.get_local_authorization_list_number_of_entries();
                    this->context.device_model.set_read_only_value(
                        local_entries.component, local_entries.variable.value(), AttributeEnum::Actual,
                        std::to_string(entries), VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL);
                } catch (const DeviceModelError& e) {
                    EVLOG_warning << "Could not set local list count to device model:" << e.what();
                } catch (const common::DatabaseException& e) {
                    EVLOG_warning << "Could not get local list count from database: " << e.what();
                } catch (const std::exception& e) {
                    EVLOG_warning << "Could not get local list count from database: " << e.what();
                }
            }
        } catch (const common::DatabaseException& e) {
            EVLOG_warning << "Could not update local authorization list in database: " << e.what();
            response.status = SendLocalListStatusEnum::Failed;
        }
    }

    ocpp::CallResult<SendLocalListResponse> call_result(response, call.uniqueId);
    this->context.message_dispatcher.dispatch_call_result(call_result);
}

void ocpp::v2::Authorization::handle_get_local_authorization_list_version_req(Call<GetLocalListVersionRequest> call) {
    GetLocalListVersionResponse response;

    if (this->context.device_model.get_optional_value<bool>(ControllerComponentVariables::LocalAuthListCtrlrEnabled)
            .value_or(false)) {
        try {
            response.versionNumber = this->context.database_handler.get_local_authorization_list_version();
        } catch (const common::DatabaseException& e) {
            const auto call_error = CallError(call.uniqueId, "InternalError",
                                              "Unable to retrieve LocalListVersion from the database", json({}));
            this->context.message_dispatcher.dispatch_call_error(call_error);
            return;
        }
    } else {
        response.versionNumber = 0;
    }

    ocpp::CallResult<GetLocalListVersionResponse> call_result(response, call.uniqueId);
    this->context.message_dispatcher.dispatch_call_result(call_result);
}

ocpp::v2::SendLocalListStatusEnum
ocpp::v2::Authorization::apply_local_authorization_list(const SendLocalListRequest& request) {
    auto status = SendLocalListStatusEnum::Failed;

    if (request.versionNumber <= 0) {
        // D01.FR.18: Do nothing, not allowed, respond with failed
    } else if (request.updateType == UpdateEnum::Full) {
        if (!request.localAuthorizationList.has_value() or request.localAuthorizationList.value().empty()) {
            try {
                this->context.database_handler.clear_local_authorization_list();
                status = SendLocalListStatusEnum::Accepted;
            } catch (const common::DatabaseException& e) {
                status = SendLocalListStatusEnum::Failed;
                EVLOG_warning << "Clearing of local authorization list failed: " << e.what();
            }
        } else {
            const auto& list = request.localAuthorizationList.value();

            if (!has_duplicate_in_list(list) and
                std::find_if(list.begin(), list.end(), has_no_token_info) == list.end()) {
                try {
                    this->context.database_handler.clear_local_authorization_list();
                    this->context.database_handler.insert_or_update_local_authorization_list(list);
                    status = SendLocalListStatusEnum::Accepted;
                } catch (const common::DatabaseException& e) {
                    status = SendLocalListStatusEnum::Failed;
                    EVLOG_warning << "Full update of local authorization list failed (at least partially): "
                                  << e.what();
                }
            }
        }
    } else if (request.updateType == UpdateEnum::Differential) {
        if (request.versionNumber <= this->context.database_handler.get_local_authorization_list_version()) {
            // D01.FR.19: Do not allow version numbers smaller than current to update differentially
            status = SendLocalListStatusEnum::VersionMismatch;
        } else if (!request.localAuthorizationList.has_value() or request.localAuthorizationList.value().empty()) {
            // D01.FR.05: Do not update database with empty list, only update version number
            status = SendLocalListStatusEnum::Accepted;
        } else if (has_duplicate_in_list(request.localAuthorizationList.value())) {
            // Do nothing with duplicate in list
        } else {
            const auto& list = request.localAuthorizationList.value();
            try {
                this->context.database_handler.insert_or_update_local_authorization_list(list);
                status = SendLocalListStatusEnum::Accepted;
            } catch (const common::DatabaseException& e) {
                status = SendLocalListStatusEnum::Failed;
                EVLOG_warning << "Differential update of authorization list failed (at least partially): " << e.what();
            }
        }
    }
    return status;
}

static bool has_duplicate_in_list(const std::vector<ocpp::v2::AuthorizationData>& list) {
    for (auto it1 = list.begin(); it1 != list.end(); ++it1) {
        for (auto it2 = it1 + 1; it2 != list.end(); ++it2) {
            if (it1->idToken.idToken == it2->idToken.idToken and it1->idToken.type == it2->idToken.type) {
                return true;
            }
        }
    }
    return false;
}

static bool has_no_token_info(const ocpp::v2::AuthorizationData& item) {
    return !item.idTokenInfo.has_value();
};
