#ifndef LPC_USE_CASE_EVENT_READER_HPP
#define LPC_USE_CASE_EVENT_READER_HPP

#include <condition_variable>
#include <memory>

// everest core deps
#include <grpcpp/grpcpp.h>

// generated
#include <control_service/control_service.grpc.pb.h>
#include <usecases/cs/lpc/service.grpc.pb.h>

#include <EebusCallbacks.hpp>

namespace module {

class EEBUS;

class LpcUseCaseEventReader : public grpc::ClientReadReactor<control_service::SubscribeUseCaseEventsResponse> {
public:
    LpcUseCaseEventReader(std::shared_ptr<control_service::ControlService::Stub> control_service_stub,
                          std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> cs_lpc_stub,
                          control_service::SubscribeUseCaseEventsRequest request, eebus::EEBusCallbacks callbacks);
    void OnReadDone(bool okay) override;
    void OnDone(const grpc::Status& status) override;
    grpc::Status Await();

private:
    void callback_write_approval_required();
    void callback_data_update_limit();
    void handle_use_case_event(control_service::SubscribeUseCaseEventsResponse& res);

    const control_service::SubscribeUseCaseEventsRequest request;
    grpc::ClientContext context;
    control_service::SubscribeUseCaseEventsResponse use_case_event;
    std::mutex mutex;
    grpc::Status status;
    bool done = false;
    std::condition_variable cv;
    std::shared_ptr<control_service::ControlService::Stub> control_service_stub;
    std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> cs_lpc_stub;
    control_service::UseCase lpc_use_case;
    eebus::EEBusCallbacks callbacks;
};

} // namespace module

#endif // LPC_USE_CASE_EVENT_READER_HPP
