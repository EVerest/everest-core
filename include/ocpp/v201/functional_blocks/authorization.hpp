// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <ocpp/v201/messages/Authorize.hpp>
#include <ocpp/v201/messages/ClearCache.hpp>
#include <ocpp/v201/messages/GetLocalListVersion.hpp>
#include <ocpp/v201/messages/SendLocalList.hpp>

#include <ocpp/v201/database_handler.hpp>
#include <ocpp/v201/message_dispatcher.hpp>
#include <ocpp/v201/message_handler.hpp>

namespace ocpp::v201 {
class AuthorizationInterface : public MessageHandlerInterface {
public:
    virtual ~AuthorizationInterface() {
    }

    virtual void start_auth_cache_cleanup_thread() = 0;
    virtual AuthorizeResponse authorize_req(const IdToken id_token, const std::optional<CiString<5500>>& certificate,
                                            const std::optional<std::vector<OCSPRequestData>>& ocsp_request_data) = 0;
    virtual void trigger_authorization_cache_cleanup() = 0;
    ///\brief Calculate and update the authorization cache size in the device model
    ///
    virtual void update_authorization_cache_size() = 0;
    virtual bool is_auth_cache_ctrlr_enabled() = 0;
    virtual void authorization_cache_insert_entry(const std::string& id_token_hash,
                                                  const IdTokenInfo& id_token_info) = 0;
    virtual std::optional<AuthorizationCacheEntry> authorization_cache_get_entry(const std::string& id_token_hash) = 0;
    virtual void authorization_cache_delete_entry(const std::string& id_token_hash) = 0;

    /// \brief Validates provided \p id_token \p certificate and \p ocsp_request_data using CSMS, AuthCache or AuthList
    /// \param id_token
    /// \param certificate
    /// \param ocsp_request_data
    /// \return AuthorizeResponse containing the result of the validation
    virtual AuthorizeResponse validate_token(const IdToken id_token, const std::optional<CiString<5500>>& certificate,
                                             const std::optional<std::vector<OCSPRequestData>>& ocsp_request_data) = 0;
};

class Authorization : public AuthorizationInterface {
private: // Members
    MessageDispatcherInterface<MessageType>& message_dispatcher;
    DeviceModel& device_model;
    ConnectivityManagerInterface& connectivity_manager;
    ocpp::v201::DatabaseHandlerInterface& database_handler;
    EvseSecurity& evse_security;

    // threads and synchronization
    bool auth_cache_cleanup_required;
    std::condition_variable auth_cache_cleanup_cv;
    std::mutex auth_cache_cleanup_mutex;
    std::thread auth_cache_cleanup_thread;
    std::atomic_bool auth_cache_cleanup_handler_running;

public:
    Authorization(MessageDispatcherInterface<MessageType>& message_dispatcher, DeviceModel& device_model,
                  ConnectivityManagerInterface& connectivity_manager, DatabaseHandlerInterface& database_handler,
                  EvseSecurity& evse_security);
    ~Authorization();
    void start_auth_cache_cleanup_thread() override;
    void handle_message(const ocpp::EnhancedMessage<MessageType>& message) override;
    AuthorizeResponse authorize_req(const IdToken id_token, const std::optional<ocpp::CiString<5500>>& certificate,
                                    const std::optional<std::vector<OCSPRequestData>>& ocsp_request_data) override;
    void trigger_authorization_cache_cleanup() override;
    void update_authorization_cache_size() override;
    bool is_auth_cache_ctrlr_enabled() override;
    void authorization_cache_insert_entry(const std::string& id_token_hash, const IdTokenInfo& id_token_info) override;
    std::optional<AuthorizationCacheEntry> authorization_cache_get_entry(const std::string& id_token_hash) override;
    void authorization_cache_delete_entry(const std::string& id_token_hash) override;

    /// \brief Validates provided \p id_token \p certificate and \p ocsp_request_data using CSMS, AuthCache or AuthList
    /// \param id_token
    /// \param certificate
    /// \param ocsp_request_data
    /// \return AuthorizeResponse containing the result of the validation
    AuthorizeResponse validate_token(const IdToken id_token, const std::optional<CiString<5500>>& certificate,
                                     const std::optional<std::vector<OCSPRequestData>>& ocsp_request_data) override;

private: // Functions
    void stop_auth_cache_cleanup_thread();

    // Functional Block C: Authorization
    void handle_clear_cache_req(Call<ClearCacheRequest> call);
    void cache_cleanup_handler();

    // Functional Block D: Local authorization list management
    void handle_send_local_authorization_list_req(Call<SendLocalListRequest> call);
    void handle_get_local_authorization_list_version_req(Call<GetLocalListVersionRequest> call);

    ///\brief Apply a local list request to the database if allowed
    ///
    ///\param request The local list request to apply
    ///\retval Accepted if applied, otherwise will return either Failed or VersionMismatch
    SendLocalListStatusEnum apply_local_authorization_list(const SendLocalListRequest& request);
};
} // namespace ocpp::v201
