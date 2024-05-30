// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V16_TRANSACTION_HPP
#define OCPP_V16_TRANSACTION_HPP

#include <memory>
#include <random>

#include <everest/timer.hpp>
#include <ocpp/v16/ocpp_types.hpp>
#include <ocpp/v16/types.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace ocpp {
namespace v16 {

/// \brief A structure that contains a energy value in Wh that can be used for start/stop energy values and a
/// corresponding timestamp
struct StampedEnergyWh {
    ocpp::DateTime timestamp; ///< A timestamp associated with the energy value
    double energy_Wh;         ///< The energy value in Wh
    StampedEnergyWh(ocpp::DateTime timestamp, double energy_Wh) {
        this->timestamp = timestamp;
        this->energy_Wh = energy_Wh;
    }
};

/// \brief Contains all transaction related data, such as the ID and power meter values
class Transaction {
private:
    std::optional<int32_t> transaction_id;
    int32_t internal_transaction_id;
    int32_t connector;
    std::string session_id;
    CiString<20> id_token;
    std::shared_ptr<StampedEnergyWh> start_energy_wh;
    std::optional<int32_t> reservation_id;
    bool active;
    bool finished;
    bool has_signed_meter_values;
    std::unique_ptr<Everest::SteadyTimer> meter_values_sample_timer;
    std::string start_transaction_message_id;
    std::string stop_transaction_message_id;
    std::shared_ptr<StampedEnergyWh> stop_energy_wh;
    std::mutex meter_values_mutex;
    std::vector<MeterValue> meter_values;

public:
    /// \brief Creates a new Transaction object, taking ownership of the provided \p meter_values_sample_timer
    /// on the provided \p connector
    Transaction(const int32_t transaction_id, const int32_t& connector, const std::string& session_id,
                const CiString<20>& id_token, const double meter_start, std::optional<int32_t> reservation_id,
                const ocpp::DateTime& timestamp, std::unique_ptr<Everest::SteadyTimer> meter_values_sample_timer);

    /// \brief Provides the energy in Wh at the start of the transaction
    /// \returns the energy in Wh combined with a timestamp
    std::shared_ptr<StampedEnergyWh> get_start_energy_wh();

    /// \brief Adds the energy in Wh \p stop_energy_wh to the transaction. This also
    /// stops the collection of further meter values.
    void add_stop_energy_wh(std::shared_ptr<StampedEnergyWh> stop_energy_wh);

    /// \brief Provides the energy in Wh at the end of the transaction
    /// \returns the energy in Wh combined with a timestamp
    std::shared_ptr<StampedEnergyWh> get_stop_energy_wh();

    /// \brief Provides the reservation id of the transaction if present
    /// \returns the reservation id
    std::optional<int32_t> get_reservation_id();

    /// \brief Provides the connector of this transaction
    /// \returns the connector
    int32_t get_connector();

    /// \brief Provides the authorized id tag of this Transaction
    /// \returns the authorized id tag
    CiString<20> get_id_tag();

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
    std::optional<int32_t> get_transaction_id();

    /// \brief Returns the internal transaction id
    int32_t get_internal_transaction_id();

    /// \brief Provides the id of this session
    /// \returns the session_id
    std::string get_session_id();

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

    /// \brief Indicates if the transaction is active. Active means that the transaction for this session is not null
    /// and no StopTransaction.req has been pushed to the message queue yet
    bool is_active();

    /// \brief Indicates if a StopTransaction.req for this transaction has already been pushed to the message queue
    bool is_finished();

    /// \brief Sets the finished flag for this transaction. This is done when a StopTransaction.req has been pushed to
    /// the message queue
    void set_finished();

    /// \brief Sets the has_signed_meter_value flag for this transaction, this function is called
    /// from \p on_transaction_started and \p on_transaction_stopped
    void set_has_signed_meter_values();

    /// \brief Indicates if this transaction has signed meter values or not, this function is called
    /// from \p on_transaction_started and \p on_transaction_stopped
    /// \returns a boolean value indicating if this transaction has signed meter values or not
    bool get_has_signed_meter_values();
};

/// \brief Contains transactions for all available connectors and manages access to these transactions
class TransactionHandler {
private:
    std::mutex active_transactions_mutex;
    // size is equal to the number of connectors
    int32_t number_of_connectors;

    std::vector<std::shared_ptr<Transaction>> active_transactions;
    // size does not depend on the number of connectors
    std::vector<std::shared_ptr<Transaction>> stopped_transactions;

    std::mt19937 gen;
    std::uniform_int_distribution<int32_t> distr;

public:
    /// \brief Creates and manages transactions for the provided \p number_of_connectors
    explicit TransactionHandler(int32_t number_of_connectors);

    /// \brief Returns a negative random transaction_id
    int32_t get_negative_random_transaction_id();

    /// \brief Adds the given \p transaction the vector of transactions
    void add_transaction(std::shared_ptr<Transaction> transaction);

    /// \brief Adds the transaction at the \p connector to the vector of stopped transactions
    void add_stopped_transaction(int32_t connector);

    /// \brief Removes a transaction from the provided \p connector
    /// \returns true if successful
    bool remove_active_transaction(int32_t connector);

    /// \brief Erases a transaction with the provided \p stop_transaction_message_id
    void erase_stopped_transaction(std::string stop_transaction_message_id);

    /// \brief Returns the transaction associated with the transaction at the provided \p connector
    /// \returns The associated transaction if available or nullptr if not
    std::shared_ptr<Transaction> get_transaction(int32_t connector);

    /// \brief Returns the transaction associated with the transaction with the provided
    /// \p start_transaction_message_id
    /// \returns The associated transaction if available or nullptr if not
    std::shared_ptr<Transaction> get_transaction(const std::string& start_transaction_message_id);

    /// \brief Provides the connector on which a transaction with the given \p transaction_id is running
    /// \returns The connector or -1 if the transaction_id is unknown
    int32_t get_connector_from_transaction_id(int32_t transaction_id);

    /// \brief Adds a clock aligned \p meter_value to the transaction on the provided \p connector
    void add_meter_value(int32_t connector, const MeterValue& meter_value);

    /// \brief Modifies the sample interval of the meter values sample timer on all connectors. The
    /// provided \p interval is expected to be given in seconds.
    void change_meter_values_sample_intervals(int32_t interval);

    // \brief Provides the IdTag that was associated with the transaction with the provided
    /// \p stop_transaction_message_id
    /// \returns the IdTag if it is available, std::nullopt otherwise
    std::optional<CiString<20>> get_authorized_id_tag(const std::string& stop_transaction_message_id);

    /// \brief Indicates if there is an active transaction at the proveded \p connector
    /// \returns true if a transaction exists
    bool transaction_active(int32_t connector);
};

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_TRANSACTION_HPP
