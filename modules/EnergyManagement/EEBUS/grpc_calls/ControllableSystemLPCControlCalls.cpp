#include <grpc_calls/ControllableSystemLPCControlCalls.hpp>

#include <utility>

namespace module {
namespace grpc_calls {

constexpr int64_t MODULE_EEBUS_FAILSAFE_DURATION_MINIMUM_NS =
    7.2e12; // 2 * 60 * 60 * 1000 * 1000 * 1000 // 2 hours in nanoseconds

ControllableSystemLPCControlCalls::ControllableSystemLPCControlCalls(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub>& stub) :
    stub(stub) {
}

void ControllableSystemLPCControlCalls::call_set_consumption_nominal_max(const double& nominal_max) {
    cs_lpc::SetConsumptionNominalMaxRequest request = cs_lpc::CreateSetConsumptionNominalMaxRequest(nominal_max);
    cs_lpc::SetConsumptionNominalMaxResponse response;
    cs_lpc::CallSetConsumptionNominalMax(this->stub, request, &response);
}

void ControllableSystemLPCControlCalls::call_set_consumption_limit(const double& limit) {
    // TODO(configurable): Make this configurable
    // We should allow setting some of these constraints via config
    common_types::LoadLimit load_limit = common_types::CreateLoadLimit(0, true, false, limit, false);
    cs_lpc::SetConsumptionLimitRequest request = cs_lpc::CreateSetConsumptionLimitRequest(&load_limit);
    cs_lpc::SetConsumptionLimitResponse response;
    cs_lpc::CallSetConsumptionLimit(this->stub, request, &response);

    std::ignore = request.release_load_limit();
}

void ControllableSystemLPCControlCalls::call_set_failsafe_consumption_active_power_limit(const double& limit) {
    // TODO(configurable): improvement configurable
    cs_lpc::SetFailsafeConsumptionActivePowerLimitRequest request =
        cs_lpc::CreateSetFailsafeConsumptionActivePowerLimitRequest(limit, true);
    cs_lpc::SetFailsafeConsumptionActivePowerLimitResponse response;
    cs_lpc::CallSetFailsafeConsumptionActivePowerLimit(this->stub, request, &response);
}

void ControllableSystemLPCControlCalls::call_set_failsafe_duration_minimum() {
    // TODO(configurable): improvement configurable
    cs_lpc::SetFailsafeDurationMinimumRequest request =
        cs_lpc::CreateSetFailsafeDurationMinimumRequest(MODULE_EEBUS_FAILSAFE_DURATION_MINIMUM_NS, true);
    cs_lpc::SetFailsafeDurationMinimumResponse response;
    cs_lpc::CallSetFailsafeDurationMinimum(this->stub, request, &response);
}

} // namespace grpc_calls
} // namespace module
