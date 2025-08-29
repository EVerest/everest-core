// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <atomic>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <chrono>
#include <exception>
#include <functional>
#include <future>
#include <optional>
#include <utility>

namespace everest::lib::companion {

namespace internal {
template <class T> T toInternalApi(T x) {
    return x;
}
static const auto empty_payload = json();
} // namespace internal

using namespace std::chrono_literals;

template <class ValueT> class AsyncApiRequestReply {
public:
    AsyncApiRequestReply(Everest::MqttProvider& mqtt_, Topics const& topic_generator_,
                         std::chrono::seconds timeout_ = 5s) :
        mqtt(mqtt_), topic_generator(topic_generator_), timeout(timeout_) {
    }
    std::optional<ValueT> create(std::string const& topic_id, json payload = json()) {
        using namespace std::literals::chrono_literals;
        using weak_promise = std::weak_ptr<std::promise<ValueT>>;
        auto reply_topic = topic_generator.reply_to_everest(boost::uuids::to_string(uuid_create()));
        auto value_prom = std::make_shared<std::promise<ValueT>>();
        auto value_fut = value_prom->get_future();
        auto unsubscribe =
            mqtt.subscribe(reply_topic, [value_prom = weak_promise(value_prom)](const auto& raw_data) mutable {
                try {
                    auto shared = value_prom.lock();
                    if (shared) {
                        auto data = json::parse(raw_data);
                        shared->set_value(data);
                    } else {
                        EVLOG_info << "Request expired before arrival of response!\n\n" << raw_data;
                    }

                } catch (std::exception const& e) {
                    EVLOG_info << "Could not parse: Exception!\n" << e.what() << "\n\n" << raw_data;
                } catch (...) {
                    EVLOG_info << "Could not parse: Reply with valid data!\n\n" << raw_data;
                }
            });
        json req;
        req["headers"]["replyTo"] = reply_topic;
        if (not payload.empty()) {
            req["payload"] = payload;
        }
        mqtt.publish(topic_generator.everest_to_extern(topic_id), req.dump());

        std::optional<ValueT> result = std::nullopt;
        if (value_fut.wait_for(timeout) == std::future_status::ready) {
            result = value_fut.get();
        }
        unsubscribe();
        return result;
    }

private:
    Everest::MqttProvider& mqtt;
    Topics const& topic_generator;
    std::chrono::seconds timeout;
    boost::uuids::random_generator uuid_create;
};

template <class ReplyT, class ReqT>
auto request_reply_handler(Everest::MqttProvider& mqtt, Topics const& topic_generator, ReqT const& request,
                           std::string const& topic, int timeout_s) {
    using namespace internal;
    using ResultT = std::optional<decltype(toInternalApi(std::declval<ReplyT>()))>;
    ResultT result = std::nullopt;
    auto timeout = timeout_s < 1 ? std::chrono::seconds(999999) : std::chrono::seconds(timeout_s);
    AsyncApiRequestReply<ReplyT> requestReply(mqtt, topic_generator, timeout);
    auto reply = requestReply.create(topic, request);
    if (reply) {
        result.emplace(toInternalApi(reply.value()));
    } else {
        EVLOG_warning << "topic: '" << topic + "' -> Reply to requested topic" << std::endl;
    }
    return result;
}

template <class ReplyT>
auto request_reply_handler(Everest::MqttProvider& mqtt, Topics const& topic_generator, std::string const& topic,
                           int timeout_s) {
    return request_reply_handler<ReplyT>(mqtt, topic_generator, internal::empty_payload, topic, timeout_s);
}

} // namespace everest::lib::companion
