// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#pragma once

#include <gmock/gmock.h>

#include <evse_mock.hpp>
#include <ocpp/v2/evse_manager.hpp>

namespace ocpp::v2 {

class EvseManagerFake : public EvseManagerInterface {
private:
    using EvseIteratorImpl = VectorOfUniquePtrIterator<EvseInterface>;

    std::vector<std::unique_ptr<EvseInterface>> evses;
    std::vector<std::unique_ptr<EnhancedTransaction>> transactions;

    DatabaseHandler db_handler{nullptr, ""}; // Only used to allow opening of transactions, will crash if actually used

public:
    explicit EvseManagerFake(size_t nr_of_evses) {
        transactions.resize(nr_of_evses);
        for (size_t i = 0; i < nr_of_evses; i++) {
            auto mock = std::make_unique<EvseMock>();
            ON_CALL(*mock, get_id).WillByDefault(testing::Return(i + 1));
            ON_CALL(*mock, get_transaction).WillByDefault(testing::ReturnRef(this->transactions.at(i)));
            ON_CALL(*mock, has_active_transaction()).WillByDefault(testing::Return(false));
            evses.push_back(std::move(mock));
        }
    }

    EvseIterator begin() override {
        return EvseIterator(std::make_unique<EvseIteratorImpl>(this->evses.begin()));
    }
    EvseIterator end() override {
        return EvseIterator(std::make_unique<EvseIteratorImpl>(this->evses.end()));
    }

    EvseInterface& get_evse(int32_t id) override {
        if (id > this->evses.size()) {
            throw EvseOutOfRangeException(id);
        }
        return *this->evses.at(id - 1);
    }

    const EvseInterface& get_evse(int32_t id) const override {
        if (id > this->evses.size()) {
            throw EvseOutOfRangeException(id);
        }
        return *this->evses.at(id - 1);
    }

    virtual bool does_connector_exist(const int32_t evse_id, ConnectorEnum connector_type) const override {
        if (evse_id > this->evses.size()) {
            return false;
        }

        return get_evse(evse_id).does_connector_exist(connector_type);
    }

    bool does_evse_exist(int32_t id) const override {
        return id <= this->evses.size();
    }

    bool are_all_connectors_effectively_inoperative() const override {
        for (const auto& evse : this->evses) {
            for (int connector_id = 1; connector_id <= evse->get_number_of_connectors(); connector_id++) {
                OperationalStatusEnum connector_status = evse->get_connector_effective_operational_status(connector_id);
                if (connector_status == OperationalStatusEnum::Operative) {
                    return false;
                }
            }
        }
        return true;
    }

    size_t get_number_of_evses() const override {
        return this->evses.size();
    }

    std::optional<int32_t> get_transaction_evseid(const CiString<36>& transaction_id) const override {
        for (const auto& evse : this->evses) {
            if (evse->has_active_transaction()) {
                if (transaction_id == evse->get_transaction()->get_transaction().transactionId) {
                    return evse->get_id();
                }
            }
        }

        return std::nullopt;
    }

    void open_transaction(int evse_id, const std::string& transaction_id) {
        auto& transaction = this->transactions.at(evse_id - 1);
        transaction = std::make_unique<EnhancedTransaction>(db_handler, false);
        transaction->transactionId = transaction_id;

        auto& mock = this->get_mock(evse_id);
        EXPECT_CALL(mock, get_transaction).WillRepeatedly(testing::ReturnRef(this->transactions.at(evse_id - 1)));
        EXPECT_CALL(mock, has_active_transaction()).WillRepeatedly(testing::Return(true));
    }

    EvseMock& get_mock(int32_t evse_id) {
        return dynamic_cast<EvseMock&>(*evses.at(evse_id - 1).get());
    }

    bool any_transaction_active(const std::optional<EVSE>& /*evse*/) const override {
        return false;
    }

    bool is_valid_evse(const EVSE& evse) const override {
        return (static_cast<int32_t>(evses.size()) > evse.id);
    }
};

} // namespace ocpp::v2
