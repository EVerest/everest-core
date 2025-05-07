#ifndef USE_CASE_EVENT_READER_HPP
#define USE_CASE_EVENT_READER_HPP

#include <future>
#include <memory>

// everest core deps
#include <grpcpp/grpcpp.h>

// generated
#include <control_service/control_service.grpc.pb.h>
#include <usecases/cs/lpc/service.grpc.pb.h>

namespace module {

class EEBUS;

class UseCaseEventReader : public grpc::ClientReadReactor<control_service::SubscribeUseCaseEventsResponse> {
public:
    UseCaseEventReader(std::shared_ptr<control_service::ControlService::Stub> control_service_stub,
                       std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> cs_lpc_stub,
                       const control_service::SubscribeUseCaseEventsRequest& request, module::EEBUS* ev_module);
    void OnReadDone(bool ok) override;
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
    module::EEBUS* ev_module;
};

} // namespace module

#endif // USE_CASE_EVENT_READER_HPP
