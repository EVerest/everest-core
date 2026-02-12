#ifndef USECASEEVENTREADER_HPP
#define USECASEEVENTREADER_HPP

#include <functional>
#include <memory>

#include <control_service/control_service.grpc.pb.h>

class UseCaseEventReader : public grpc::ClientReadReactor<control_service::SubscribeUseCaseEventsResponse> {
public:
    UseCaseEventReader(std::shared_ptr<control_service::ControlService::Stub> stub,
                       std::function<void(const control_service::UseCaseEvent&)> event_callback);

    void start(const common_types::EntityAddress& entity_address, const control_service::UseCase& use_case);
    void stop();

    void OnReadDone(bool ok) override;
    void OnDone(const grpc::Status& s) override;

private:
    std::shared_ptr<control_service::ControlService::Stub> stub;
    std::function<void(const control_service::UseCaseEvent&)> event_callback;

    grpc::ClientContext context;
    control_service::SubscribeUseCaseEventsRequest request;
    control_service::SubscribeUseCaseEventsResponse response;
};

#endif // USECASEEVENTREADER_HPP
