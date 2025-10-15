#include "LpcUseCaseEventReader.hpp"

#include <date/date.h>

// generated
#include <usecases/cs/lpc/service.grpc-ext.pb.h>

// module internal
#include <EebusCallbacks.hpp>
#include <helper.hpp>

namespace module {

LpcUseCaseEventReader::LpcUseCaseEventReader(
    std::shared_ptr<control_service::ControlService::Stub> control_service_stub,
    std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> cs_lpc_stub,
    control_service::SubscribeUseCaseEventsRequest request, eebus::EEBusCallbacks callbacks) :
    control_service_stub(std::move(control_service_stub)),
    cs_lpc_stub(std::move(cs_lpc_stub)),
    request(std::move(request)),
    lpc_use_case(control_service::CreateUseCase(control_service::UseCase_ActorType_Enum_ControllableSystem,
                                                control_service::UseCase_NameType_Enum_limitationOfPowerConsumption)),
    callbacks(std::move(callbacks)) {
    this->control_service_stub->async()->SubscribeUseCaseEvents(&this->context, &this->request, this);
    this->StartRead(&this->use_case_event);
    this->StartCall();
}

void LpcUseCaseEventReader::OnReadDone(bool ok) {
    if (ok) {
        this->handle_use_case_event(this->use_case_event);
        this->StartRead(&this->use_case_event);
    } else {
        EVLOG_error << "Could not read use case event";
        EVLOG_error << "Stop reading use case events";
    }
}

void LpcUseCaseEventReader::OnDone(const grpc::Status& status) {
    std::unique_lock<std::mutex> lock(this->mutex);
    this->status = status;
    this->done = true;
    this->cv.notify_one();
}

grpc::Status LpcUseCaseEventReader::Await() {
    std::unique_lock<std::mutex> lock(this->mutex);
    this->cv.wait(lock, [this] { return this->done; });
    return std::move(this->status);
}

void LpcUseCaseEventReader::callback_write_approval_required() {
    cs_lpc::PendingConsumptionLimitRequest request = cs_lpc::CreatePendingConsumptionLimitRequest();
    cs_lpc::PendingConsumptionLimitResponse response;
    cs_lpc::CallPendingConsumptionLimit(this->cs_lpc_stub, request, &response);

    auto pending_writes = response.load_limits();
    for (const auto& entry : pending_writes) {
        uint64_t msg_counter = entry.first;
        cs_lpc::ApproveOrDenyConsumptionLimitRequest request_02 =
            cs_lpc::CreateApproveOrDenyConsumptionLimitRequest(msg_counter, true, "");
        cs_lpc::ApproveOrDenyConsumptionLimitResponse response_02;
        cs_lpc::CallApproveOrDenyConsumptionLimit(this->cs_lpc_stub, request_02, &response_02);
    }
}

void LpcUseCaseEventReader::callback_data_update_limit() {
    cs_lpc::ConsumptionLimitRequest request = cs_lpc::CreateConsumptionLimitRequest();
    cs_lpc::ConsumptionLimitResponse response;
    cs_lpc::CallConsumptionLimit(this->cs_lpc_stub, request, &response);
    common_types::LoadLimit load_limit = response.load_limit();
    EVLOG_debug << "Data Update Limit, load limit:\n" << load_limit.DebugString();

    types::energy::ExternalLimits limits;
    limits = translate_to_external_limits(load_limit);
    this->callbacks.update_limits_callback(limits);
}

void LpcUseCaseEventReader::handle_use_case_event(control_service::SubscribeUseCaseEventsResponse& res) {
    if (!compare_use_case(lpc_use_case, res.use_case_event().use_case())) {
        return;
    }

    std::string event = res.use_case_event().event();
    if (event == "DataUpdateHeartbeat") {
        EVLOG_debug << "ignore hearbeat";
    } else if (event == "UseCaseSupportUpdate") {
        EVLOG_debug << "ignore use case support update";
    } else if (event == "WriteApprovalRequired") {
        this->callback_write_approval_required();
    } else if (event == "DataUpdateLimit") {
        this->callback_data_update_limit();
    } else {
        EVLOG_error << "Oh never heard about: " << event;
    }
}

} // namespace module
