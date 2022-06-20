// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_CHARGING_SESSION_HPP
#define OCPP1_6_CHARGING_SESSION_HPP

#include <memory>

#include <ocpp1_6/ocpp_types.hpp>
#include <ocpp1_6/types.hpp>

namespace ocpp1_6 {

/// \brief Contains all transaction related data, such as the ID and power meter values
class Transaction {
private:
    int32_t transactionId;
    bool active;
    std::unique_ptr<Everest::SteadyTimer> meter_values_sample_timer;
    std::mutex sampled_meter_values_mutex;
    std::vector<MeterValue> sampled_meter_values;
    std::mutex clock_aligned_meter_values_mutex;
    std::vector<MeterValue> clock_aligned_meter_values;
    std::mutex tx_charging_profiles_mutex;
    std::map<int32_t, ChargingProfile> tx_charging_profiles;

public:
    /// \brief Creates a new Transaction object, taking ownership of the provided \p meter_values_sample_timer
    Transaction(int32_t transactionId, std::unique_ptr<Everest::SteadyTimer> meter_values_sample_timer);

    /// \brief Adds the provided \p meter_value to a chronological list of sampled powermeter values
    void add_sampled_meter_value(MeterValue meter_value);

    /// \brief Provides all recorded sampled powermeter values
    /// \returns a vector of sampled powermeter values
    std::vector<MeterValue> get_sampled_meter_values();

    /// \brief Changes the sample \p interval of the powermeter values sampling timer
    /// \returns true if successful
    bool change_meter_values_sample_interval(int32_t interval);

    /// \brief Adds the provided \p meter_value to a chronological list of clock aligned powermeter values
    void add_clock_aligned_meter_value(MeterValue meter_value);

    /// \brief Provides all recorded clock aligned powermeter values
    /// \returns a vector of clock aligned powermeter values
    std::vector<MeterValue> get_clock_aligned_meter_values();

    /// \brief Provides the id of this transaction
    /// \returns the transaction id
    int32_t get_transaction_id();

    /// \brief Provides all recorded sampled and clock aligned powermeter values
    /// \returns a vector of sampled and clock aligned powermeter values packaged into a TransactionData object
    std::vector<TransactionData> get_transaction_data();

    /// \brief Marks the transaction as stopped/inactive
    void stop();

    /// \brief Set a \p charging_profile
    void set_charging_profile(ChargingProfile charging_profile);

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

    /// \brief Adds a sampled \p meter_value to the charging session
    void add_sampled_meter_value(MeterValue meter_value);

    /// \brief Provides a list of sampled meter values
    /// \returns A vector of associated sampled meter values
    std::vector<MeterValue> get_sampled_meter_values();

    /// \brief Adds a clock aligned \p meter_value to the charging session
    void add_clock_aligned_meter_value(MeterValue meter_value);

    /// \brief Provides a list of clock aligned meter values
    /// \returns A vector of associated clock aligned meter values
    std::vector<MeterValue> get_clock_aligned_meter_values();

    /// \brief Adds the given \p reservation_id to the charging session
    void add_reservation_id(int32_t reservation_id);

    /// \brief Provides the reservation id of the charging session
    /// \returns the reservation id if it is available, boost::none otherwise
    boost::optional<int32_t> get_reservation_id();
};

/// \brief Contains charging sessions for all available connectors and manages access to these charging sessions
class ChargingSessions {
private:
    int32_t number_of_connectors;
    std::mutex charging_sessions_mutex;
    std::vector<std::unique_ptr<ChargingSession>> charging_sessions;

    /// \brief Indicates if the given \p connector is in the valid range between 0 and the number of connectors
    /// \returns true if the connector is within the valid range
    bool valid_connector(int32_t connector);

public:
    /// \brief Creates and manages charging sessions for the provided \p number_of_connectors
    explicit ChargingSessions(int32_t number_of_connectors);

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
    bool remove_session(int32_t connector);

    /// \brief Indicates if the charging session on the provided \p connector is ready
    /// \returns true if the charging session is ready, false if not
    bool ready(int32_t connector);

    /// \brief Adds a transaction to the charging session at the provided \p connector
    /// \returns true if this was successful, false if a transaction is already associated with this charging session
    bool add_transaction(int32_t connector, std::shared_ptr<Transaction> transaction);

    /// \brief Returns the transaction associated with the charging session at the provided \p connector
    /// \returns The associated transaction if available or nullptr if not
    std::shared_ptr<Transaction> get_transaction(int32_t connector);

    /// \brief Indicates if there is an active transaction at the proveded \p connector
    /// \returns true if a transaction exists
    bool transaction_active(int32_t connector);

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

    /// \brief Adds a sampled \p meter_value to the charging session at the given \p connector
    void add_sampled_meter_value(int32_t connector, MeterValue meter_value);

    /// \brief Provides a list of sampled meter values from the given \p connector
    /// \returns A vector of associated sampled meter values
    std::vector<MeterValue> get_sampled_meter_values(int32_t connector);

    /// \brief Adds a clock aligned \p meter_value to the charging session on the provided \p connector
    void add_clock_aligned_meter_value(int32_t connector, MeterValue meter_value);

    /// \brief Provides a list of clock aligned meter values from the given \p connector
    /// \returns A vector of associated clock aligned meter values
    std::vector<MeterValue> get_clock_aligned_meter_values(int32_t connector);

    /// \brief Adds a \p reservation_id to the charging session on the provided \p connector
    void add_reservation_id(int32_t connector, int32_t reservation_id);

    /// \brief Provides the reservation id from the given \p connector
    /// \returns the reservation id if it is available, boost::none otherwise
    boost::optional<int32_t> get_reservation_id(int32_t connector);
};

} // namespace ocpp1_6

#endif // OCPP1_6_CHARGING_SESSION_HPP
