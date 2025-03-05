
#ifndef EVEREST_CORE_MODULES_EEBUS_INCLUDE_GRPC_CALLS_CONTROLSERVICECALLS_HPP
#define EVEREST_CORE_MODULES_EEBUS_INCLUDE_GRPC_CALLS_CONTROLSERVICECALLS_HPP

// generated
#include <control_service/control_service.grpc-ext.pb.h>
#include <usecases/cs/lpc/service.grpc-ext.pb.h>

namespace module {

class EEBUS;

namespace grpc_calls {

class ControlServiceCalls {
    public:
        ControlServiceCalls(
            const std::shared_ptr<control_service::ControlService::Stub> stub
        );

        void call_set_config();
        void call_start_setup();
        void call_register_remote_ski(const std::string& remote_ski);
        std::string call_add_use_case();

        void subscribe_use_case_events(
            module::EEBUS* eebus_module,
            const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> cs_lpc_stub
        );

        void call_start_service();
    private:
        const std::shared_ptr<control_service::ControlService::Stub> stub;
        common_types::EntityAddress entity_address;
        control_service::UseCase use_case;
};

} // namespace grpc_calls
} // namespace module

#endif // EVEREST_CORE_MODULES_EEBUS_INCLUDE_GRPC_CALLS_CONTROLSERVICECALLS_HPP
