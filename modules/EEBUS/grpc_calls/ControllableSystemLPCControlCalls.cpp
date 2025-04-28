#include <grpc_calls/ControllableSystemLPCControlCalls.hpp>

#include <utility>

namespace module {
namespace grpc_calls {

ControllableSystemLPCControlCalls::ControllableSystemLPCControlCalls(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub) :
    stub(stub) {
}

void ControllableSystemLPCControlCalls::call_set_consumption_nominal_max() {
    cs_lpc::SetConsumptionNominalMaxRequest request = cs_lpc::CreateSetConsumptionNominalMaxRequest(32000);
    cs_lpc::SetConsumptionNominalMaxResponse response;
    cs_lpc::CallSetConsumptionNominalMax(this->stub, request, &response);
}

void ControllableSystemLPCControlCalls::call_set_consumption_limit() {
    common_types::LoadLimit load_limit = common_types::CreateLoadLimit(0, true, false, 4200, false);
    cs_lpc::SetConsumptionLimitRequest request = cs_lpc::CreateSetConsumptionLimitRequest(&load_limit);
    cs_lpc::SetConsumptionLimitResponse response;
    cs_lpc::CallSetConsumptionLimit(this->stub, request, &response);

    std::ignore = request.release_load_limit();
}

void ControllableSystemLPCControlCalls::call_set_failsafe_consumption_active_power_limit() {
    cs_lpc::SetFailsafeConsumptionActivePowerLimitRequest request =
        cs_lpc::CreateSetFailsafeConsumptionActivePowerLimitRequest(4200, true);
    cs_lpc::SetFailsafeConsumptionActivePowerLimitResponse response;
    cs_lpc::CallSetFailsafeConsumptionActivePowerLimit(this->stub, request, &response);
}

void ControllableSystemLPCControlCalls::call_set_failsafe_duration_minimum() {
    cs_lpc::SetFailsafeDurationMinimumRequest request =
        cs_lpc::CreateSetFailsafeDurationMinimumRequest((int64_t)2 * 60 * 60 * 1000 * 1000 * 1000, true);
    cs_lpc::SetFailsafeDurationMinimumResponse response;
    cs_lpc::CallSetFailsafeDurationMinimum(this->stub, request, &response);
}

} // namespace grpc_calls
} // namespace module
