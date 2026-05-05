// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef USECASEEVENTREADER_HPP
#define USECASEEVENTREADER_HPP

#include <functional>
#include <future>
#include <memory>

#include <control_service/control_service.grpc.pb.h>

class UseCaseEventReader : public grpc::ClientReadReactor<control_service::SubscribeUseCaseEventsResponse> {
public:
    UseCaseEventReader(std::shared_ptr<control_service::ControlService::Stub> stub,
                       std::function<void(const control_service::SubscribeUseCaseEventsResponse&)> event_callback,
                       std::function<void()> disconnection_callback);
    ~UseCaseEventReader();
    UseCaseEventReader(const UseCaseEventReader&) = delete;
    UseCaseEventReader(UseCaseEventReader&&) = delete;
    UseCaseEventReader& operator=(const UseCaseEventReader&) = delete;
    UseCaseEventReader& operator=(UseCaseEventReader&&) = delete;

    void start(const common_types::EntityAddress& entity_address, const control_service::UseCase& use_case);
    void stop();

    void OnReadDone(bool ok) override;
    void OnDone(const grpc::Status& s) override;

private:
    std::shared_ptr<control_service::ControlService::Stub> stub;
    std::function<void(const control_service::SubscribeUseCaseEventsResponse&)> event_callback;
    std::function<void()> disconnection_callback;

    grpc::ClientContext context;
    control_service::SubscribeUseCaseEventsRequest request;
    control_service::SubscribeUseCaseEventsResponse response;

    bool started_{false};
    std::promise<void> done_promise_;
    std::future<void> done_future_;
};

#endif // USECASEEVENTREADER_HPP
