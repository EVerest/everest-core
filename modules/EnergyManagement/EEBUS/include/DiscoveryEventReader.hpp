// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef MODULES_ENERGYMANAGEMENT_EEBUS_INCLUDE_DISCOVERYEVENTREADER_HPP
#define MODULES_ENERGYMANAGEMENT_EEBUS_INCLUDE_DISCOVERYEVENTREADER_HPP

#include <atomic>
#include <functional>
#include <future>
#include <memory>

#include <control_service/control_service.grpc.pb.h>

class DiscoveryEventReader : public grpc::ClientReadReactor<control_service::DiscoveryEvent> {
public:
    using EventCallback = std::function<void(const control_service::DiscoveryEvent&)>;
    using DisconnectionCallback = std::function<void()>;

    DiscoveryEventReader(std::shared_ptr<control_service::ControlService::Stub> stub, EventCallback event_callback,
                         DisconnectionCallback disconnection_callback);
    ~DiscoveryEventReader();
    DiscoveryEventReader(const DiscoveryEventReader&) = delete;
    DiscoveryEventReader(DiscoveryEventReader&&) = delete;
    DiscoveryEventReader& operator=(const DiscoveryEventReader&) = delete;
    DiscoveryEventReader& operator=(DiscoveryEventReader&&) = delete;

    void start();
    void stop();

    void OnReadDone(bool ok) override;
    void OnDone(const grpc::Status& s) override;

private:
    std::shared_ptr<control_service::ControlService::Stub> stub;
    EventCallback event_callback;
    DisconnectionCallback disconnection_callback;

    grpc::ClientContext context;
    control_service::SubscribeDiscoveryEventsRequest request;
    control_service::DiscoveryEvent response;

    std::atomic<bool> started_{false};
    std::atomic<bool> stopped_{false};
    std::promise<void> done_promise_;
    std::future<void> done_future_;
};

#endif // MODULES_ENERGYMANAGEMENT_EEBUS_INCLUDE_DISCOVERYEVENTREADER_HPP
