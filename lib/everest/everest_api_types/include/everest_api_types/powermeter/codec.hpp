// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "API.hpp"
#include <optional>
#include <string>

namespace everest::lib::API::V1_0::types::powermeter {

std::string serialize(OCMFUserIdentificationStatus val) noexcept;
std::string serialize(OCMFIdentificationFlags val) noexcept;
std::string serialize(OCMFIdentificationType val) noexcept;
std::string serialize(OCMFIdentificationLevel val) noexcept;
std::string serialize(Current const& val) noexcept;
std::string serialize(Voltage const& val) noexcept;
std::string serialize(Frequency const& val) noexcept;
std::string serialize(Power const& val) noexcept;
std::string serialize(Energy const& val) noexcept;
std::string serialize(ReactivePower const& val) noexcept;
std::string serialize(SignedMeterValue const& val) noexcept;
std::string serialize(SignedCurrent const& val) noexcept;
std::string serialize(SignedVoltage const& val) noexcept;
std::string serialize(SignedFrequency const& val) noexcept;
std::string serialize(SignedPower const& val) noexcept;
std::string serialize(SignedEnergy const& val) noexcept;
std::string serialize(SignedReactivePower const& val) noexcept;
std::string serialize(Temperature const& val) noexcept;
std::string serialize(PowermeterValues const& val) noexcept;
std::string serialize(TransactionStatus val) noexcept;
std::string serialize(ReplyStartTransaction const& val) noexcept;
std::string serialize(ReplyStopTransaction const& val) noexcept;
std::string serialize(RequestStartTransaction const& val) noexcept;

std::ostream& operator<<(std::ostream& os, OCMFUserIdentificationStatus const& val);
std::ostream& operator<<(std::ostream& os, OCMFIdentificationFlags const& val);
std::ostream& operator<<(std::ostream& os, OCMFIdentificationType const& val);
std::ostream& operator<<(std::ostream& os, OCMFIdentificationLevel const& val);
std::ostream& operator<<(std::ostream& os, Current const& val);
std::ostream& operator<<(std::ostream& os, Voltage const& val);
std::ostream& operator<<(std::ostream& os, Frequency const& val);
std::ostream& operator<<(std::ostream& os, Power const& val);
std::ostream& operator<<(std::ostream& os, Energy const& val);
std::ostream& operator<<(std::ostream& os, ReactivePower const& val);
std::ostream& operator<<(std::ostream& os, SignedMeterValue const& val);
std::ostream& operator<<(std::ostream& os, SignedCurrent const& val);
std::ostream& operator<<(std::ostream& os, SignedVoltage const& val);
std::ostream& operator<<(std::ostream& os, SignedFrequency const& val);
std::ostream& operator<<(std::ostream& os, SignedPower const& val);
std::ostream& operator<<(std::ostream& os, SignedEnergy const& val);
std::ostream& operator<<(std::ostream& os, SignedReactivePower const& val);
std::ostream& operator<<(std::ostream& os, Temperature const& val);
std::ostream& operator<<(std::ostream& os, PowermeterValues const& val);
std::ostream& operator<<(std::ostream& os, TransactionStatus const& val);
std::ostream& operator<<(std::ostream& os, ReplyStartTransaction const& val);
std::ostream& operator<<(std::ostream& os, ReplyStopTransaction const& val);
std::ostream& operator<<(std::ostream& os, RequestStartTransaction const& val);

template <class T> T deserialize(std::string const& val);
template <class T> std::optional<T> try_deserialize(std::string const& val) noexcept {
    try {
        return deserialize<T>(val);
    } catch (...) {
        return std::nullopt;
    }
}
template <class T> bool adl_deserialize(std::string const& json_data, T& obj) {
    auto opt = try_deserialize<T>(json_data);
    if (opt) {
        obj = opt.value();
        return true;
    }
    return false;
}

} // namespace everest::lib::API::V1_0::types::powermeter
