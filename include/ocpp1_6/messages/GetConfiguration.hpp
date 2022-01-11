// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_GETCONFIGURATION_HPP
#define OCPP1_6_GETCONFIGURATION_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 GetConfiguration message
struct GetConfigurationRequest : public Message {
    boost::optional<std::vector<CiString50Type>> key;

    /// \brief Provides the type of this GetConfiguration message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "GetConfiguration";
    }

    /// \brief Conversion from a given GetConfigurationRequest \p k to a given json object \p j
    friend void to_json(json& j, const GetConfigurationRequest& k) {
        // the required parts of the message
        j = json({});
        // the optional parts of the message
        if (k.key) {
            if (j.size() == 0) {
                j = json{{"key", json::array()}};
            } else {
                j["key"] = json::array();
            }
            for (auto val : k.key.value()) {
                j["key"].push_back(val);
            }
        }
    }

    /// \brief Conversion from a given json object \p j to a given GetConfigurationRequest \p k
    friend void from_json(const json& j, GetConfigurationRequest& k) {
        // the required parts of the message

        // the optional parts of the message
        if (j.contains("key")) {
            json arr = j.at("key");
            std::vector<CiString50Type> vec;
            for (auto val : arr) {
                vec.push_back(val);
            }
            k.key.emplace(vec);
        }
    }

    /// \brief Writes the string representation of the given GetConfigurationRequest \p k to the given output stream \p
    /// os \returns an output stream with the GetConfigurationRequest written to
    friend std::ostream& operator<<(std::ostream& os, const GetConfigurationRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

/// \brief Contains a OCPP 1.6 GetConfigurationResponse message
struct GetConfigurationResponse : public Message {
    boost::optional<std::vector<KeyValue>> configurationKey;
    boost::optional<std::vector<CiString50Type>> unknownKey;

    /// \brief Provides the type of this GetConfigurationResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "GetConfigurationResponse";
    }

    /// \brief Conversion from a given GetConfigurationResponse \p k to a given json object \p j
    friend void to_json(json& j, const GetConfigurationResponse& k) {
        // the required parts of the message
        j = json({});
        // the optional parts of the message
        if (k.configurationKey) {
            if (j.size() == 0) {
                j = json{{"configurationKey", json::array()}};
            } else {
                j["configurationKey"] = json::array();
            }
            for (auto val : k.configurationKey.value()) {
                j["configurationKey"].push_back(val);
            }
        }
        if (k.unknownKey) {
            if (j.size() == 0) {
                j = json{{"unknownKey", json::array()}};
            } else {
                j["unknownKey"] = json::array();
            }
            for (auto val : k.unknownKey.value()) {
                j["unknownKey"].push_back(val);
            }
        }
    }

    /// \brief Conversion from a given json object \p j to a given GetConfigurationResponse \p k
    friend void from_json(const json& j, GetConfigurationResponse& k) {
        // the required parts of the message

        // the optional parts of the message
        if (j.contains("configurationKey")) {
            json arr = j.at("configurationKey");
            std::vector<KeyValue> vec;
            for (auto val : arr) {
                vec.push_back(val);
            }
            k.configurationKey.emplace(vec);
        }
        if (j.contains("unknownKey")) {
            json arr = j.at("unknownKey");
            std::vector<CiString50Type> vec;
            for (auto val : arr) {
                vec.push_back(val);
            }
            k.unknownKey.emplace(vec);
        }
    }

    /// \brief Writes the string representation of the given GetConfigurationResponse \p k to the given output stream \p
    /// os \returns an output stream with the GetConfigurationResponse written to
    friend std::ostream& operator<<(std::ostream& os, const GetConfigurationResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_GETCONFIGURATION_HPP
