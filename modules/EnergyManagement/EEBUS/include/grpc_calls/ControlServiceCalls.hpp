
#ifndef EVEREST_CORE_MODULES_EEBUS_INCLUDE_GRPC_CALLS_CONTROLSERVICECALLS_HPP
#define EVEREST_CORE_MODULES_EEBUS_INCLUDE_GRPC_CALLS_CONTROLSERVICECALLS_HPP

// generated
#include <control_service/control_service.grpc-ext.pb.h>
#include <usecases/cs/lpc/service.grpc-ext.pb.h>

#include <EebusCallbacks.hpp>
#include <LpcUseCaseEventReader.hpp>

namespace module {

class EEBUS;

namespace grpc_calls {

using use_case_stubs = std::variant<std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub>>;

class ControlServiceCalls {
public:
    explicit ControlServiceCalls(const std::shared_ptr<control_service::ControlService::Stub>& stub);

    void call_set_config(uint32_t port, std::string vendor_code, std::string device_brand, std::string device_model,
                         std::string serial_number);
    void call_start_setup();
    void call_register_remote_ski(const std::string& remote_ski);
    std::string call_add_use_case(const control_service::UseCase& use_case);

    void subscribe_use_case_events(eebus::EEBusCallbacks callbacks, use_case_stubs stub);

    void call_start_service();

private:
    std::shared_ptr<control_service::ControlService::Stub> control_service_stub;
    common_types::EntityAddress entity_address;
    std::unique_ptr<LpcUseCaseEventReader> lpc_uc_reader;
    control_service::UseCase lpc_use_case;
};

} // namespace grpc_calls
} // namespace module

#endif // EVEREST_CORE_MODULES_EEBUS_INCLUDE_GRPC_CALLS_CONTROLSERVICECALLS_HPP
