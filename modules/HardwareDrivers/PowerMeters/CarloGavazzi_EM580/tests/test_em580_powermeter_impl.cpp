// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>

#include "helper.hpp"
#include "powermeterImpl.hpp"

#include <deque>
#include <map>
#include <string_view>
#include <tuple>

namespace {

constexpr std::size_t kFetchCallAlignmentBytes = 8;
constexpr std::size_t kWriteCallAlignmentBytes = 32;

struct alignas(kFetchCallAlignmentBytes) FetchCall {
    std::int32_t address;
    std::uint16_t register_count;
};

struct alignas(kWriteCallAlignmentBytes) WriteCall {
    std::int32_t address;
    std::vector<std::uint16_t> data;
};

class FakeModbusTransport : public transport::AbstractModbusTransport {
public:
    void push_fetch_response(std::int32_t address, std::uint16_t register_count, transport::DataVector response) {
        scripted_fetch_[Key{address, register_count}].push_back(std::move(response));
    }

    const std::vector<FetchCall>& fetch_calls() const {
        return fetch_calls_;
    }

    const std::vector<WriteCall>& write_calls() const {
        return write_calls_;
    }

    transport::DataVector fetch(std::int32_t address, std::uint16_t register_count) override {
        fetch_calls_.push_back(FetchCall{address, register_count});

        const Key key{address, register_count};
        auto iter = scripted_fetch_.find(key);
        if (iter == scripted_fetch_.end() || iter->second.empty()) {
            throw std::runtime_error("FakeModbusTransport: no scripted fetch response for address/count");
        }
        transport::DataVector out = std::move(iter->second.front());
        iter->second.pop_front();
        return out;
    }

    void write_multiple_registers(std::int32_t address, const std::vector<std::uint16_t>& data) override {
        write_calls_.push_back(WriteCall{address, data});
    }

private:
    using Key = std::tuple<std::int32_t, std::uint16_t>;
    std::map<Key, std::deque<transport::DataVector>> scripted_fetch_;
    std::vector<FetchCall> fetch_calls_;
    std::vector<WriteCall> write_calls_;
};

transport::DataVector u16_be(std::uint16_t value) {
    constexpr std::uint32_t kByteBits = 8U;
    constexpr std::uint32_t kByteMask = 0xFFU;
    const auto high = static_cast<std::uint8_t>((static_cast<std::uint32_t>(value) >> kByteBits) & kByteMask);
    const auto low = static_cast<std::uint8_t>(static_cast<std::uint32_t>(value) & kByteMask);
    return transport::DataVector{high, low};
}

transport::DataVector bytes(std::string_view str) {
    return transport::DataVector{str.begin(), str.end()};
}

module::main::Conf make_test_conf() {
    constexpr int kDefaultIntervalMs = 1000;
    module::main::Conf conf{};
    conf.powermeter_device_id = 1;
    conf.communication_retry_count = 0;
    conf.communication_retry_delay_ms = 0;
    conf.initial_connection_retry_count = 0;
    conf.initial_connection_retry_delay_ms = 0;
    conf.timezone_offset_minutes = 0;
    conf.live_measurement_interval_ms = kDefaultIntervalMs;
    conf.device_state_read_interval_ms = kDefaultIntervalMs;
    conf.communication_error_pause_delay_s = 0;
    return conf;
}

} // namespace

TEST(EM580PowermeterImpl, StartTransactionHappyPathCountsWrites) {
    static Everest::PtrContainer<module::CarloGavazzi_EM580> dummy_mod;
    auto conf = make_test_conf();
    module::main::powermeterImpl impl(nullptr, dummy_mod, conf);

    auto transport = std::make_unique<FakeModbusTransport>();
    transport->push_fetch_response(em580::registers::MODBUS_OCMF_STATE_ADDRESS, 1,
                                   u16_be(em580::registers::MODBUS_OCMF_STATE_NOT_READY));

    auto* transport_ptr = transport.get();
    module::main::powermeterImpl::TestAccess::set_modbus_transport(impl, std::move(transport));

    types::powermeter::TransactionReq req{};
    req.evse_id = "DE*TEST*EVSE01";
    req.transaction_id = "12345678-1234-5678-1234-567812345678";
    req.identification_status = types::powermeter::OCMFUserIdentificationStatus::ASSIGNED;
    req.identification_flags = {};
    req.identification_type = types::powermeter::OCMFIdentificationType::ISO14443;
    req.identification_level = types::powermeter::OCMFIdentificationLevel::NONE;
    req.identification_data.emplace("ABC");
    req.tariff_text.emplace("TARIFF");

    const auto resp = module::main::powermeterImpl::TestAccess::start_transaction(impl, req);
    EXPECT_EQ(resp.status, types::powermeter::TransactionRequestStatus::OK);

    // Happy-path expectation: 8 writes from write_transaction_registers + 2
    // additional writes (session modality + 'B')
    EXPECT_EQ(transport_ptr->write_calls().size(), 10U);
}

TEST(EM580PowermeterImpl, StopTransactionPendingClosedTransactionMismatchReturnsError) {
    static Everest::PtrContainer<module::CarloGavazzi_EM580> dummy_mod;
    auto conf = make_test_conf();
    module::main::powermeterImpl impl(nullptr, dummy_mod, conf);

    auto transport = std::make_unique<FakeModbusTransport>();
    // read_ocmf_file(): size then file bytes
    transport->push_fetch_response(em580::registers::MODBUS_OCMF_STATE_SIZE_ADDRESS, 1, u16_be(1));

    const std::string other_uuid = "aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa";
    const std::string ocmf_file = R"(OCMF|{"TT":"x<=>)" + other_uuid + R"("}|
{"SA":"ECDSA-brainpoolP384r1-SHA256","SD":"00"})";
    transport->push_fetch_response(em580::registers::MODBUS_OCMF_STATE_FILE_ADDRESS, 1, bytes(ocmf_file));

    auto* transport_ptr = transport.get();
    module::main::powermeterImpl::TestAccess::set_modbus_transport(impl, std::move(transport));
    module::main::powermeterImpl::TestAccess::set_pending_closed_transaction(impl, true);

    std::string requested_uuid = "12345678-1234-5678-1234-567812345678";
    const auto resp = module::main::powermeterImpl::TestAccess::stop_transaction(impl, requested_uuid);
    EXPECT_EQ(resp.status, types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR);
    ASSERT_TRUE(resp.error.has_value());
    EXPECT_EQ(*resp.error, "Transaction id mismatch");
    EXPECT_EQ(transport_ptr->write_calls().size(), 0U);
}

TEST(EM580PowermeterImpl, StopTransactionEmptyIdWithoutPendingClosedTransactionCleansUpAndReturnsOk) {
    static Everest::PtrContainer<module::CarloGavazzi_EM580> dummy_mod;
    auto conf = make_test_conf();
    module::main::powermeterImpl impl(nullptr, dummy_mod, conf);

    auto transport = std::make_unique<FakeModbusTransport>();
    // clear_transaction_states() fetches OCMF state once; use NOT_READY to avoid
    // extra cleanup behavior.
    transport->push_fetch_response(em580::registers::MODBUS_OCMF_STATE_ADDRESS, 1,
                                   u16_be(em580::registers::MODBUS_OCMF_STATE_NOT_READY));

    auto* transport_ptr = transport.get();
    module::main::powermeterImpl::TestAccess::set_modbus_transport(impl, std::move(transport));
    module::main::powermeterImpl::TestAccess::set_transaction_id(impl, "12345678-1234-5678-1234-567812345678");
    module::main::powermeterImpl::TestAccess::set_pending_closed_transaction(impl, false);

    std::string empty_id;
    const auto resp = module::main::powermeterImpl::TestAccess::stop_transaction(impl, empty_id);
    EXPECT_EQ(resp.status, types::powermeter::TransactionRequestStatus::OK);

    // Expect one write ('E' end command) when no pending closed transaction
    // exists.
    EXPECT_EQ(transport_ptr->write_calls().size(), 1U);
}

TEST(EM580PowermeterImpl, StartTransactionSpuriousReadyStateDoesCleanupAndNoTransactionWrites) {
    static Everest::PtrContainer<module::CarloGavazzi_EM580> dummy_mod;
    auto conf = make_test_conf();
    module::main::powermeterImpl impl(nullptr, dummy_mod, conf);

    auto transport = std::make_unique<FakeModbusTransport>();
    // handle_start_transaction() first checks OCMF state.
    transport->push_fetch_response(em580::registers::MODBUS_OCMF_STATE_ADDRESS, 1,
                                   u16_be(em580::registers::MODBUS_OCMF_STATE_READY));
    // clear_transaction_states(): reads state again, sees READY, reads file
    // (size+file) and confirms NOT_READY.
    transport->push_fetch_response(em580::registers::MODBUS_OCMF_STATE_ADDRESS, 1,
                                   u16_be(em580::registers::MODBUS_OCMF_STATE_READY));
    transport->push_fetch_response(em580::registers::MODBUS_OCMF_STATE_SIZE_ADDRESS, 1, u16_be(1));
    transport->push_fetch_response(em580::registers::MODBUS_OCMF_STATE_FILE_ADDRESS, 1,
                                   bytes("OCMF|{\"TT\":\"x<=>12345678-1234-5678-1234-567812345678\"}|{}"));

    auto* transport_ptr = transport.get();
    module::main::powermeterImpl::TestAccess::set_modbus_transport(impl, std::move(transport));

    types::powermeter::TransactionReq req{};
    req.evse_id = "DE*TEST*EVSE01";
    req.transaction_id = "12345678-1234-5678-1234-567812345678";
    req.identification_status = types::powermeter::OCMFUserIdentificationStatus::ASSIGNED;
    req.identification_flags = {};
    req.identification_type = types::powermeter::OCMFIdentificationType::ISO14443;
    req.identification_level = types::powermeter::OCMFIdentificationLevel::NONE;
    req.identification_data.emplace("ABC");
    req.tariff_text.emplace("TARIFF");

    const auto resp = module::main::powermeterImpl::TestAccess::start_transaction(impl, req);
    EXPECT_EQ(resp.status, types::powermeter::TransactionRequestStatus::OK);

    // Only the cleanup confirm write should happen here (no transaction register
    // writes).
    EXPECT_EQ(transport_ptr->write_calls().size(), 1U);
    EXPECT_EQ(transport_ptr->write_calls()[0].address, em580::registers::MODBUS_OCMF_STATE_ADDRESS);
}

TEST(EM580PowermeterImpl, StopTransactionUnknownIdReturnsErrorAndNoWrites) {
    static Everest::PtrContainer<module::CarloGavazzi_EM580> dummy_mod;
    auto conf = make_test_conf();
    module::main::powermeterImpl impl(nullptr, dummy_mod, conf);

    auto transport = std::make_unique<FakeModbusTransport>();
    auto* transport_ptr = transport.get();
    module::main::powermeterImpl::TestAccess::set_modbus_transport(impl, std::move(transport));
    module::main::powermeterImpl::TestAccess::set_pending_closed_transaction(impl, false);
    module::main::powermeterImpl::TestAccess::set_transaction_id(impl, "11111111-1111-1111-1111-111111111111");

    std::string requested_uuid = "22222222-2222-2222-2222-222222222222";
    const auto resp = module::main::powermeterImpl::TestAccess::stop_transaction(impl, requested_uuid);
    EXPECT_EQ(resp.status, types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR);
    ASSERT_TRUE(resp.error.has_value());
    EXPECT_EQ(*resp.error, "No open transaction or unknown transaction id");
    EXPECT_TRUE(transport_ptr->write_calls().empty());
}
