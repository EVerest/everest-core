// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_CHARGING_SESSION_HPP
#define OCPP1_6_CHARGING_SESSION_HPP

#include <memory>

#include <ocpp1_6/ocpp_types.hpp>
#include <ocpp1_6/types.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace ocpp1_6 {

/// \brief Contains all transaction related data, such as the ID and power meter values
class Transaction {
private:
    int32_t transactionId;
    int32_t connector;
    std::string start_transaction_message_id;
    std::string stop_transaction_message_id;
    bool active;
    bool finished;
    CiString20Type idTag;
    std::unique_ptr<Everest::SteadyTimer> meter_values_sample_timer;
    std::mutex meter_values_mutex;
    std::vector<MeterValue> meter_values;
    std::mutex tx_charging_profiles_mutex;
    std::map<int32_t, ChargingProfile> tx_charging_profiles;

public:
    /// \brief Creates a new Transaction object, taking ownership of the provided \p meter_values_sample_timer
    /// on the provided \p connector
    Transaction(int32_t transactionId, int32_t connector,
                std::unique_ptr<Everest::SteadyTimer> meter_values_sample_timer, CiString20Type idTag,
                std::string start_transaction_message_id);

    /// \brief Provides the connector of this transaction
    /// \returns the connector
    int32_t get_connector();

    /// \brief Provides the authorized id tag of this Transaction
    /// \returns the authorized id tag
    CiString20Type get_id_tag();

    /// \brief Adds the provided \p meter_value to a chronological list of powermeter values
    void add_meter_value(MeterValue meter_value);

    /// \brief Provides all recorded powermeter values
    /// \returns a vector of powermeter values
    std::vector<MeterValue> get_meter_values();

    /// \brief Changes the sample \p interval of the powermeter values sampling timer
    /// \returns true if successful
    bool change_meter_values_sample_interval(int32_t interval);

    /// \brief Adds the provided \p meter_value to a chronological list of clock aligned powermeter values
    void add_clock_aligned_meter_value(MeterValue meter_value);

    /// \brief Provides the id of this transaction
    /// \returns the transaction id
    int32_t get_transaction_id();

    /// \brief Sets the start transaction message id using the provides \p message_id
    void set_start_transaction_message_id(const std::string& message_id);

    /// \brief Provides the start transaction message id
    std::string get_start_transaction_message_id();

    /// \brief Sets the stop transaction message id using the provides \p message_id
    void set_stop_transaction_message_id(const std::string& message_id);

    /// \brief Provides the stop transaction message id
    std::string get_stop_transaction_message_id();

    /// \brief Sets the transaction id
    void set_transaction_id(int32_t transaction_id);

    /// \brief Provides all recorded sampled and clock aligned powermeter values
    /// \returns a vector of sampled and clock aligned powermeter values packaged into a TransactionData object
    std::vector<TransactionData> get_transaction_data();

    /// \brief Marks the transaction as stopped/inactive
    void stop();

    /// \brief Set a \p charging_profile
    void set_charging_profile(ChargingProfile charging_profile);

    /// \brief Indicates if the transaction is active. Active means that the transaction for this session is not null
    /// and no StopTransaction.req has been pushed to the message queue yet
    bool transaction_active();

    /// \brief Indicates if a StopTransaction.req for this transaction has already been pushed to the message queue
    bool is_finished();

    /// \brief Sets the finished flag for this transaction. This is done when a StopTransaction.req has been pushed to
    /// the message queue
    void set_finished();

    /// \brief Remove the charging profile at the provided \p stack_level
    void remove_charging_profile(int32_t stack_level);

    /// \brief Remove all charging profiles
    void remove_charging_profiles();

    /// \brief \returns all charging profiles of this transaction
    std::map<int32_t, ChargingProfile> get_charging_profiles();
};

/// \brief A structure that contains a idTag and its corresponding idTagInfo
struct AuthorizedToken {
    CiString20Type idTag; ///< The authorization token
    IdTagInfo idTagInfo;  ///< Information about the authorization status of the idTag authorization token
    /// \brief Creates a AuthorizedToken from an \p idTag and \p idTagInfo
    AuthorizedToken(CiString20Type idTag, IdTagInfo idTagInfo) {
        this->idTag = idTag;
        this->idTagInfo = idTagInfo;
    }
};

/// \brief A structure that contains a energy value in Wh that can be used for start/stop energy values and a
/// corresponding timestamp
struct StampedEnergyWh {
    DateTime timestamp; ///< A timestamp associated with the energy value
    double energy_Wh;   ///< The energy value in Wh
    StampedEnergyWh(DateTime timestamp, double energy_Wh) {
        this->timestamp = timestamp;
        this->energy_Wh = energy_Wh;
    }
};

/// \brief Manages a charging session
class ChargingSession {
private:
    std::unique_ptr<AuthorizedToken> authorized_token; // the presented authentication
    std::shared_ptr<StampedEnergyWh> start_energy_wh;
    std::shared_ptr<StampedEnergyWh> stop_energy_wh;
    boost::optional<int32_t> reservation_id;
    bool plug_connected;                      // if the EV is plugged in or not
    std::shared_ptr<Transaction> transaction; // once all conditions for charging are met this contains the transaction

public:
    /// \brief Creates a charging session without authorization information
    ChargingSession();

    /// \brief Creates a charging session with the provided \p authorized_token
    explicit ChargingSession(std::unique_ptr<AuthorizedToken> authorized_token);

    ~ChargingSession() {
        this->transaction = nullptr;
    }

    /// \brief Signals that an EV was plugged in
    void connect_plug();

    /// \brief Signals that an EV was disconnected
    void disconnect_plug();

    /// \brief Informs about the availability of an authorized token attached to this session
    /// \returns true if this session has a authorized token attached to it
    bool authorized_token_available();

    /// \brief Sets the \p authorized_token of the charging session
    /// \returns true if this was successful
    bool add_authorized_token(std::unique_ptr<AuthorizedToken> authorized_token);

    /// \brief Adds the energy in Wh at the start of the transaction \p start_energy_wh to the charging session
    /// \returns returns true if this was successful
    bool add_start_energy_wh(std::shared_ptr<StampedEnergyWh> start_energy_wh);

    /// \brief Provides the energy in Wh at the start of the transaction
    /// \returns the energy in Wh combined with a timestamp
    std::shared_ptr<StampedEnergyWh> get_start_energy_wh();

    /// \brief Adds the energy in Wh at the end of the transaction \p start_energy_wh to the charging session. This also
    /// stops the collection of further meter values.
    /// \returns returns true if this was successful
    bool add_stop_energy_wh(std::shared_ptr<StampedEnergyWh> stop_energy_wh);

    /// \brief Provides the energy in Wh at the end of the transaction
    /// \returns the energy in Wh combined with a timestamp
    std::shared_ptr<StampedEnergyWh> get_stop_energy_wh();

    /// \brief Indicates if the charging session is ready to start
    /// \returns true if the charging session can be started
    bool ready();

    /// \brief Indicates if the charging session is running
    /// \returns true if the charging session is running
    bool running();

    /// \brief Provides the IdTag that was associated with this charging session if it is available
    /// \returns the IdTag if it is available, boost::none otherwise
    boost::optional<CiString20Type> get_authorized_id_tag();

    /// \brief Adds a transaction to this charging session
    /// \returns true if this was successful, false if a transaction is already associated with this charging session
    bool add_transaction(std::shared_ptr<Transaction> transaction);

    /// \brief Provides the transaction associated with this charging session
    /// \returns The associated transaction if available or nullptr if not
    std::shared_ptr<Transaction> get_transaction();

    /// \brief Modifies the sample interval of the meter values sample timer. The provided \p interval is expected to be
    /// given in seconds. \returns true if the change was successful
    bool change_meter_values_sample_interval(int32_t interval);

    /// \brief Adds a \p meter_value to the charging session
    void add_meter_value(MeterValue meter_value);

    /// \brief Provides a list of meter values
    /// \returns A vector of associated meter values
    std::vector<MeterValue> get_meter_values();

    /// \brief Adds the given \p reservation_id to the charging session
    void add_reservation_id(int32_t reservation_id);

    /// \brief Provides the reservation id of the charging session
    /// \returns the reservation id if it is available, boost::none otherwise
    boost::optional<int32_t> get_reservation_id();
};

/// \brief Contains charging sessions for all available connectors and manages access to these charging sessions
class ChargingSessionHandler {
private:
    int32_t number_of_connectors;
    std::mutex active_charging_sessions_mutex;
    // size is equal to the number of connectors
    std::vector<std::unique_ptr<ChargingSession>> active_charging_sessions;
    // size does not depend on the number of connectors
    std::vector<std::shared_ptr<Transaction>> stopped_transactions;

    /// \brief Indicates if the given \p connector is in the valid range between 0 and the number of connectors
    /// \returns true if the connector is within the valid range
    bool valid_connector(int32_t connector);

public:
    /// \brief Creates and manages charging sessions for the provided \p number_of_connectors
    explicit ChargingSessionHandler(int32_t number_of_connectors);

    /// \brief Adds the given \p stopped_transaction to the vector of stopped transactions
    void add_stopped_transaction(std::shared_ptr<Transaction> stopped_transaction);

    /// \brief Adds an authorized token, created from the provided \p idTag and \p idTagInfo to any connector that is in
    /// need of a token
    /// \returns the connector to which the authorized token was added, -1 if no valid connector was found
    int32_t add_authorized_token(CiString20Type idTag, IdTagInfo idTagInfo);

    /// \brief Adds an authorized token, created from the provided \p idTag and \p idTagInfo to the given \p connector
    /// \returns the connector to which the authorized token was added, -1 if no valid connector was found
    int32_t add_authorized_token(int32_t connector, CiString20Type idTag, IdTagInfo idTagInfo);

    /// \brief Adds the energy in Wh at the start of the transaction \p start_energy_wh to the charging session on the
    /// given \p connector
    /// \returns returns true if this was successful
    bool add_start_energy_wh(int32_t connector, std::shared_ptr<StampedEnergyWh> start_energy_wh);

    /// \brief Provides the energy in Wh at the start of the transaction on the given \p connector
    /// \returns the energy in Wh combined with a timestamp
    std::shared_ptr<StampedEnergyWh> get_start_energy_wh(int32_t connector);

    /// \brief Adds the energy in Wh at the end of the transaction \p start_energy_wh to the charging session on the
    /// given \p connector . This also stops the collection of further meter values. \returns returns true if this was
    /// successful
    bool add_stop_energy_wh(int32_t connector, std::shared_ptr<StampedEnergyWh> stop_energy_wh);

    /// \brief Provides the energy in Wh at the end of the transaction on the given \p connector
    /// \returns the energy in Wh combined with a timestamp
    std::shared_ptr<StampedEnergyWh> get_stop_energy_wh(int32_t connector);

    /// \brief Initiates a charging session on the provided \p connector by associating a new ChargingSession object
    /// with the connector. This function also sets the connector to plugged in. \returns true if successful, false if a
    /// session is already running on the connector
    bool initiate_session(int32_t connector);

    /// \brief Removes a charging session from the provided \p connector
    /// \returns true if successful
    bool remove_active_session(int32_t connector);

    /// \brief Removes a charging session with the provided \p stop_transaction_message_i
    /// \returns true if successful
    bool remove_stopped_transaction(std::string stop_transaction_message_id);

    /// \brief Indicates if the charging session on the provided \p connector is ready
    /// \returns true if the charging session is ready, false if not
    bool ready(int32_t connector);

    /// \brief Adds a transaction to the charging session at the provided \p connector
    /// \returns true if this was successful, false if a transaction is already associated with this charging session
    bool add_transaction(int32_t connector, std::shared_ptr<Transaction> transaction);

    /// \brief Returns the transaction associated with the charging session at the provided \p connector
    /// \returns The associated transaction if available or nullptr if not
    std::shared_ptr<Transaction> get_transaction(int32_t connector);

    /// \brief Returns the transaction associated with the charging session with the provided
    /// \p start_transaction_message_id
    /// \returns The associated transaction if available or nullptr if not
    std::shared_ptr<Transaction> get_transaction(const std::string& start_transaction_message_id);

    /// \brief Indicates if there is an active transaction at the proveded \p connector
    /// \returns true if a transaction exists
    bool transaction_active(int32_t connector);

    /// \brief Indicates if there is an active transaction for all connectors
    /// \returns true if all connectors have active transactions
    bool all_connectors_have_active_transaction();

    /// \brief Provides the connector on which a transaction with the given \p transaction_id is running
    /// \returns The connector or -1 if the transaction_id is unknown
    int32_t get_connector_from_transaction_id(int32_t transaction_id);

    /// \brief Modifies the sample interval of the meter values sample timer associated with the given \p connector. The
    /// provided \p interval is expected to be given in seconds.
    /// \returns true if the change was successful
    bool change_meter_values_sample_interval(int32_t connector, int32_t interval);

    /// \brief Modifies the sample interval of the meter values sample timer on all connectors. The
    /// provided \p interval is expected to be given in seconds.
    /// \returns true if the change was successful
    bool change_meter_values_sample_intervals(int32_t interval);

    /// \brief Provides the IdTag that was associated with the charging session on the provided \p connector
    /// \returns the IdTag if it is available, boost::none otherwise
    boost::optional<CiString20Type> get_authorized_id_tag(int32_t connector);

    /// \brief Provides the IdTag that was associated with the charging session with the provided
    /// \p stop_transaction_message_id
    /// \returns the IdTag if it is available, boost::none otherwise
    boost::optional<CiString20Type> get_authorized_id_tag(std::string stop_transaction_message_id);

    /// \brief Provides a list of meter values from the given \p connector
    /// \returns A vector of associated meter values
    std::vector<MeterValue> get_meter_values(int32_t connector);

    /// \brief Adds a clock aligned \p meter_value to the charging session on the provided \p connector
    void add_meter_value(int32_t connector, MeterValue meter_value);

    /// \brief Adds a \p reservation_id to the charging session on the provided \p connector
    void add_reservation_id(int32_t connector, int32_t reservation_id);

    /// \brief Provides the reservation id from the given \p connector
    /// \returns the reservation id if it is available, boost::none otherwise
    boost::optional<int32_t> get_reservation_id(int32_t connector);
};

} // namespace ocpp1_6

#endif // OCPP1_6_CHARGING_SESSION_HPP
