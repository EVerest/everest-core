// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "powermeter/codec.hpp"
#include "nlohmann/json.hpp"
#include "powermeter/API.hpp"
#include "powermeter/json_codec.hpp"
#include "utilities/constants.hpp"
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::powermeter {

std::string serialize(OCMFUserIdentificationStatus val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(OCMFIdentificationFlags val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(OCMFIdentificationType val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(OCMFIdentificationLevel val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(Current const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(Voltage const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(Frequency const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(Power const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(Energy const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ReactivePower const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(SignedMeterValue const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(SignedCurrent const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(SignedVoltage const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(SignedFrequency const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(SignedPower const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(SignedEnergy const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(SignedReactivePower const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(PowermeterValues const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(Temperature const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(TransactionStatus val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ReplyStartTransaction const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ReplyStopTransaction const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(RequestStartTransaction const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

template <> OCMFUserIdentificationStatus deserialize(std::string const& val) {
    return json::parse(val);
}

template <> OCMFIdentificationFlags deserialize(std::string const& val) {
    return json::parse(val);
}

template <> OCMFIdentificationType deserialize(std::string const& val) {
    return json::parse(val);
}

template <> OCMFIdentificationLevel deserialize(std::string const& val) {
    return json::parse(val);
}

template <> Current deserialize(std::string const& val) {
    return json::parse(val);
}

template <> Voltage deserialize(std::string const& val) {
    return json::parse(val);
}

template <> Frequency deserialize(std::string const& val) {
    return json::parse(val);
}

template <> Power deserialize(std::string const& val) {
    return json::parse(val);
}

template <> Energy deserialize(std::string const& val) {
    return json::parse(val);
}

template <> ReactivePower deserialize(std::string const& val) {
    return json::parse(val);
}

template <> SignedMeterValue deserialize(std::string const& val) {
    return json::parse(val);
}

template <> SignedCurrent deserialize(std::string const& val) {
    return json::parse(val);
}

template <> SignedVoltage deserialize(std::string const& val) {
    return json::parse(val);
}

template <> SignedFrequency deserialize(std::string const& val) {
    return json::parse(val);
}

template <> SignedPower deserialize(std::string const& val) {
    return json::parse(val);
}

template <> SignedEnergy deserialize(std::string const& val) {
    return json::parse(val);
}

template <> SignedReactivePower deserialize(std::string const& val) {
    return json::parse(val);
}

template <> Temperature deserialize(std::string const& val) {
    return json::parse(val);
}

template <> PowermeterValues deserialize(std::string const& val) {
    return json::parse(val);
}

template <> TransactionStatus deserialize(std::string const& val) {
    return json::parse(val);
}

template <> ReplyStartTransaction deserialize(std::string const& val) {
    return json::parse(val);
}

template <> ReplyStopTransaction deserialize(std::string const& val) {
    return json::parse(val);
}

template <> RequestStartTransaction deserialize(std::string const& val) {
    return json::parse(val);
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
