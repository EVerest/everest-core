#include "UseCaseEventReader.hpp"

#include <date/date.h>
#include <chrono>

// generated
#include <usecases/cs/lpc/service.grpc-ext.pb.h>

// module internal
#include <helper.hpp>
#include "EEBUS.hpp"


namespace module {

UseCaseEventReader::UseCaseEventReader(
    std::shared_ptr<control_service::ControlService::Stub> control_service_stub,
    std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> cs_lpc_stub,
    const control_service::SubscribeUseCaseEventsRequest& request,
    module::EEBUS* ev_module
) :
    control_service_stub(control_service_stub),
    cs_lpc_stub(cs_lpc_stub),
    request(request),
    ev_module(ev_module)
{
    this->control_service_stub->async()->SubscribeUseCaseEvents(&this->context, &this->request, this);
    this->StartRead(&this->use_case_event);
    this->StartCall();
}

void UseCaseEventReader::OnReadDone(bool ok) {
    if (ok) {
        this->handle_use_case_event(this->use_case_event);
        this->StartRead(&this->use_case_event);
    } else {
        EVLOG_error << "Could not read use case event" << std::endl;
        EVLOG_error << "Stop reading use case events" << std::endl;
    }
}

void UseCaseEventReader::OnDone(const grpc::Status& status) {
    std::unique_lock<std::mutex> l(this->mutex);
    this->status = status;
    this->done = true;
    this->cv.notify_one();
}

grpc::Status UseCaseEventReader::Await() {
    std::unique_lock<std::mutex> l(this->mutex);
    this->cv.wait(
        l,
        [this] {
            return this->done;
        }
    );
    return std::move(this->status);
}

void UseCaseEventReader::callback_write_approval_required() {
    cs_lpc::PendingConsumptionLimitRequest request = cs_lpc::CreatePendingConsumptionLimitRequest();
    cs_lpc::PendingConsumptionLimitResponse response;
    cs_lpc::CallPendingConsumptionLimit(
        this->cs_lpc_stub,
        request,
        &response
    );

    auto pending_writes = response.load_limits();
    for (auto entry : pending_writes) {
        uint64_t msg_counter = entry.first;
        common_types::LoadLimit& load_limit = entry.second;

        cs_lpc::ApproveOrDenyConsumptionLimitRequest request_02 = cs_lpc::CreateApproveOrDenyConsumptionLimitRequest(
            msg_counter,
            true,
            ""
        );
        cs_lpc::ApproveOrDenyConsumptionLimitResponse response_02;
        cs_lpc::CallApproveOrDenyConsumptionLimit(
            this->cs_lpc_stub,
            request_02,
            &response_02
        );
    }
}

void UseCaseEventReader::callback_data_update_limit() {
    cs_lpc::ConsumptionLimitRequest request = cs_lpc::CreateConsumptionLimitRequest();
    cs_lpc::ConsumptionLimitResponse response;
    cs_lpc::CallConsumptionLimit(
        this->cs_lpc_stub,
        request,
        &response
    );
    common_types::LoadLimit load_limit = response.load_limit();    
    EVLOG_debug << "load limit:" << std::endl << load_limit.DebugString();

    types::energy::ExternalLimits limits;
    limits = translate_to_external_limits(load_limit);
    this->ev_module->r_eebus_energy_sink->call_set_external_limits(limits);
}

void UseCaseEventReader::handle_use_case_event(control_service::SubscribeUseCaseEventsResponse& res) {
    control_service::UseCase lpc = control_service::CreateUseCase(
        control_service::UseCase_ActorType_Enum_ControllableSystem,
        control_service::UseCase_NameType_Enum_limitationOfPowerConsumption
    );

    if (!compare_use_case(lpc, res.use_case_event().use_case())) {
        return;
    }

    std::string event = res.use_case_event().event();
    if (event == "DataUpdateHeartbeat") {
        EVLOG_debug << "ignore hearbeat" << std::endl;
        return;
    } else if (event == "UseCaseSupportUpdate") {
        EVLOG_debug << "ignore use case support update" << std::endl;
        return;
    } else if (event == "WriteApprovalRequired") {
        this->callback_write_approval_required();
        return;
    } else if (event == "DataUpdateLimit") {
        this->callback_data_update_limit();
    } else {
        EVLOG_error << "Oh never heard about: " << event << std::endl;
        return;
    }
}

} // namespace module
