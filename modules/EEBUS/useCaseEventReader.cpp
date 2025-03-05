#include "useCaseEventReader.hpp"

#include "helper.hpp"
#include <date/date.h>
#include <chrono>

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
        // std::cout << "Received use case event:" << std::endl
        // << "  remote_ski: " << this->use_case_event.remote_ski() << std::endl
        // << "  remote_entity_address: " << this->use_case_event.remote_entity_address().DebugString() << std::endl
        // << "  use_case_event: " << this->use_case_event.use_case_event().DebugString() << std::endl;
        this->handle_use_case_event(this->use_case_event);
        this->StartRead(&this->use_case_event);
    } else {
        std::cout << "read wasn't ok" << std::endl;
        exit(-1);
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
    cs_lpc::PendingConsumptionLimitRequest request;
    cs_lpc::PendingConsumptionLimitResponse response;
    grpc::ClientContext context;
    cs_lpc_stub->PendingConsumptionLimit(&context, request, &response);

    auto pending_writes = response.load_limits();
    for (auto entry : pending_writes) {
        uint64_t msg_counter = entry.first;
        common_types::LoadLimit& laod_limit = entry.second;

        cs_lpc::ApproveOrDenyConsumptionLimitRequest request_02;
        request_02.set_msg_counter(msg_counter);
        request_02.set_approve(true);
        request_02.set_reason("");
        cs_lpc::ApproveOrDenyConsumptionLimitResponse response_02;
        grpc::ClientContext context_02;
        cs_lpc_stub->ApproveOrDenyConsumptionLimit(
            &context_02,
            request_02,
            &response_02
        );

    }
}

void UseCaseEventReader::callback_data_update_limit() {
    cs_lpc::ConsumptionLimitRequest request;
    cs_lpc::ConsumptionLimitResponse response;
    grpc::ClientContext context;
    this->cs_lpc_stub->ConsumptionLimit(
        &context,
        request,
        &response
    );
    common_types::LoadLimit load_limit = response.load_limit();
    
    types::energy::ExternalLimits limits;
    std::vector<types::energy::ScheduleReqEntry> schedule_import;
    types::energy::ScheduleReqEntry schedule_req_entry;
    types::energy::LimitsReq limits_req;
    const std::chrono::time_point<date::utc_clock> timestamp =  date::utc_clock::from_sys(std::chrono::system_clock::now());
    schedule_req_entry.timestamp = Everest::Date::to_rfc3339(timestamp);
    limits_req.total_power_W = load_limit.value();
    schedule_req_entry.limits_to_leaves = limits_req;
    schedule_req_entry.limits_to_root = limits_req;
    schedule_import.push_back(schedule_req_entry);
    limits.schedule_import = schedule_import;
    this->ev_module->r_eebus_energy_sink->call_set_external_limits(limits);
    std::cout << "load limit:" << std::endl << load_limit.DebugString();
}

void UseCaseEventReader::handle_use_case_event(control_service::SubscribeUseCaseEventsResponse& res) {
    control_service::UseCase lpc;
    lpc.set_actor(control_service::UseCase_ActorType_Enum_ControllableSystem);
    lpc.set_name(control_service::UseCase_NameType_Enum_limitationOfPowerConsumption);

    if (!compare_use_case(lpc, res.use_case_event().use_case())) {
        return;
    }

    std::string event = res.use_case_event().event();
    if (event == "DataUpdateHeartbeat") {
        std::cout << "ignore hearbeat" << std::endl;
        return;
    } else if (event == "UseCaseSupportUpdate") {
        std::cout << "ignore use case support update" << std::endl;
        return;
    } else if (event == "WriteApprovalRequired") {
        this->callback_write_approval_required();
        return;
    } else if (event == "DataUpdateLimit") {
        this->callback_data_update_limit();
    } else {
        std::cout << "Oh never heard about: " << event << std::endl;
        return;
    }
}

} // namespace module
