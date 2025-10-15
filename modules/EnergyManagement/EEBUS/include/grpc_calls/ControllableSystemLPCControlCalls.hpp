
#ifndef EVEREST_CORE_MODULES_EEBUS_INCLUDE_GRPC_CALLS_CONTROLLABLESYSTEMLPCCONTROLCALLS_HPP
#define EVEREST_CORE_MODULES_EEBUS_INCLUDE_GRPC_CALLS_CONTROLLABLESYSTEMLPCCONTROLCALLS_HPP

// generated
#include <usecases/cs/lpc/service.grpc-ext.pb.h>

namespace module {
namespace grpc_calls {

class ControllableSystemLPCControlCalls {
public:
    explicit ControllableSystemLPCControlCalls(const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub>& stub);

    void call_set_consumption_nominal_max(const double& nominal_max);
    void call_set_consumption_limit(const double& limit);
    void call_set_failsafe_consumption_active_power_limit(const double& limit);
    void call_set_failsafe_duration_minimum();

private:
    std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub;
};

} // namespace grpc_calls
} // namespace module

#endif // EVEREST_CORE_MODULES_EEBUS_INCLUDE_GRPC_CALLS_CONTROLLABLESYSTEMLPCCONTROLCALLS_HPP
