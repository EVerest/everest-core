// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "powermeter/codec.hpp"
#include "nlohmann/json.hpp"
#include "powermeter/API.hpp"
#include "powermeter/json_codec.hpp"
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::powermeter {

std::string serialize(OCMFUserIdentificationStatus val) noexcept {
    json j = val;
    return j.dump(4);
}

std::string serialize(OCMFIdentificationFlags val) noexcept {
    json j = val;
    return j.dump(4);
}

std::string serialize(OCMFIdentificationType val) noexcept {
    json j = val;
    return j.dump(4);
}

std::string serialize(OCMFIdentificationLevel val) noexcept {
    json j = val;
    return j.dump(4);
}

std::string serialize(Current const& val) noexcept {
    json j = val;
    return j.dump(4);
}

std::string serialize(Voltage const& val) noexcept {
    json j = val;
    return j.dump(4);
}

std::string serialize(Frequency const& val) noexcept {
    json j = val;
    return j.dump(4);
}

std::string serialize(Power const& val) noexcept {
    json j = val;
    return j.dump(4);
}

std::string serialize(Energy const& val) noexcept {
    json j = val;
    return j.dump(4);
}

std::string serialize(ReactivePower const& val) noexcept {
    json j = val;
    return j.dump(4);
}

std::string serialize(SignedMeterValue const& val) noexcept {
    json j = val;
    return j.dump(4);
}

std::string serialize(SignedCurrent const& val) noexcept {
    json j = val;
    return j.dump(4);
}

std::string serialize(SignedVoltage const& val) noexcept {
    json j = val;
    return j.dump(4);
}

std::string serialize(SignedFrequency const& val) noexcept {
    json j = val;
    return j.dump(4);
}

std::string serialize(SignedPower const& val) noexcept {
    json j = val;
    return j.dump(4);
}

std::string serialize(SignedEnergy const& val) noexcept {
    json j = val;
    return j.dump(4);
}

std::string serialize(SignedReactivePower const& val) noexcept {
    json j = val;
    return j.dump(4);
}

std::string serialize(PowermeterValues const& val) noexcept {
    json j = val;
    return j.dump(4);
}

std::string serialize(Temperature const& val) noexcept {
    json j = val;
    return j.dump(4);
}

std::string serialize(TransactionStatus val) noexcept {
    json j = val;
    return j.dump(4);
}

std::string serialize(ReplyStartTransaction const& val) noexcept {
    json j = val;
    return j.dump(4);
}

std::string serialize(ReplyStopTransaction const& val) noexcept {
    json j = val;
    return j.dump(4);
}

std::string serialize(RequestStartTransaction const& val) noexcept {
    json j = val;
    return j.dump(4);
}

template <> OCMFUserIdentificationStatus deserialize(std::string const& s) {
    auto data = json::parse(s);
    OCMFUserIdentificationStatus result = data;
    return result;
}

template <> OCMFIdentificationFlags deserialize(std::string const& s) {
    auto data = json::parse(s);
    OCMFIdentificationFlags result = data;
    return result;
}

template <> OCMFIdentificationType deserialize(std::string const& s) {
    auto data = json::parse(s);
    OCMFIdentificationType result = data;
    return result;
}

template <> OCMFIdentificationLevel deserialize(std::string const& s) {
    auto data = json::parse(s);
    OCMFIdentificationLevel result = data;
    return result;
}

template <> Current deserialize(std::string const& s) {
    auto data = json::parse(s);
    Current result = data;
    return result;
}

template <> Voltage deserialize(std::string const& s) {
    auto data = json::parse(s);
    Voltage result = data;
    return result;
}

template <> Frequency deserialize(std::string const& s) {
    auto data = json::parse(s);
    Frequency result = data;
    return result;
}

template <> Power deserialize(std::string const& s) {
    auto data = json::parse(s);
    Power result = data;
    return result;
}

template <> Energy deserialize(std::string const& s) {
    auto data = json::parse(s);
    Energy result = data;
    return result;
}

template <> ReactivePower deserialize(std::string const& s) {
    auto data = json::parse(s);
    ReactivePower result = data;
    return result;
}

template <> SignedMeterValue deserialize(std::string const& s) {
    auto data = json::parse(s);
    SignedMeterValue result = data;
    return result;
}

template <> SignedCurrent deserialize(std::string const& s) {
    auto data = json::parse(s);
    SignedCurrent result = data;
    return result;
}

template <> SignedVoltage deserialize(std::string const& s) {
    auto data = json::parse(s);
    SignedVoltage result = data;
    return result;
}

template <> SignedFrequency deserialize(std::string const& s) {
    auto data = json::parse(s);
    SignedFrequency result = data;
    return result;
}

template <> SignedPower deserialize(std::string const& s) {
    auto data = json::parse(s);
    SignedPower result = data;
    return result;
}

template <> SignedEnergy deserialize(std::string const& s) {
    auto data = json::parse(s);
    SignedEnergy result = data;
    return result;
}

template <> SignedReactivePower deserialize(std::string const& s) {
    auto data = json::parse(s);
    SignedReactivePower result = data;
    return result;
}

template <> Temperature deserialize(std::string const& s) {
    auto data = json::parse(s);
    Temperature result = data;
    return result;
}

template <> PowermeterValues deserialize(std::string const& s) {
    auto data = json::parse(s);
    PowermeterValues result = data;
    return result;
}

template <> TransactionStatus deserialize(std::string const& s) {
    auto data = json::parse(s);
    TransactionStatus result = data;
    return result;
}

template <> ReplyStartTransaction deserialize(std::string const& s) {
    auto data = json::parse(s);
    ReplyStartTransaction result = data;
    return result;
}

template <> ReplyStopTransaction deserialize(std::string const& s) {
    auto data = json::parse(s);
    ReplyStopTransaction result = data;
    return result;
}

template <> RequestStartTransaction deserialize(std::string const& s) {
    auto data = json::parse(s);
    RequestStartTransaction result = data;
    return result;
}

std::ostream& operator<<(std::ostream& os, OCMFUserIdentificationStatus const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, OCMFIdentificationFlags const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, OCMFIdentificationType const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, OCMFIdentificationLevel const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, Current const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, Voltage const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, Frequency const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, Power const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, Energy const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ReactivePower const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, SignedMeterValue const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, SignedCurrent const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, SignedVoltage const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, SignedFrequency const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, SignedPower const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, SignedEnergy const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, SignedReactivePower const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, Temperature const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, PowermeterValues const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, TransactionStatus const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ReplyStartTransaction const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ReplyStopTransaction const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, RequestStartTransaction const& val) {
    os << serialize(val);
    return os;
}

} // namespace everest::lib::API::V1_0::types::powermeter
