// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_GETCONFIGURATION_HPP
#define OCPP1_6_GETCONFIGURATION_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {
struct GetConfigurationRequest : public Message {
    boost::optional<std::vector<CiString50Type>> key;

    std::string get_type() const {
        return "GetConfiguration";
    }

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

    friend std::ostream& operator<<(std::ostream& os, const GetConfigurationRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

struct GetConfigurationResponse : public Message {
    boost::optional<std::vector<KeyValue>> configurationKey;
    boost::optional<std::vector<CiString50Type>> unknownKey;

    std::string get_type() const {
        return "GetConfigurationResponse";
    }

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

    friend std::ostream& operator<<(std::ostream& os, const GetConfigurationResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_GETCONFIGURATION_HPP
