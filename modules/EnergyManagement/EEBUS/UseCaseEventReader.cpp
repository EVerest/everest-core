#include <include/UseCaseEventReader.hpp>

#include <everest/logging.hpp>

#include "common_types/types.pb.h"
#include "control_service/messages.pb.h"

UseCaseEventReader::UseCaseEventReader(std::shared_ptr<control_service::ControlService::Stub> stub,
                                       std::function<void(const control_service::UseCaseEvent&)> event_callback) :
    stub(std::move(stub)), event_callback(std::move(event_callback)) {
}

void UseCaseEventReader::start(const common_types::EntityAddress& entity_address,
                               const control_service::UseCase& use_case) {
    this->request.mutable_entity_address()->CopyFrom(entity_address);
    this->request.mutable_use_case()->CopyFrom(use_case);
    this->stub->async()->SubscribeUseCaseEvents(&context, &request, this);
    this->StartRead(&this->response);
    this->StartCall();
}

void UseCaseEventReader::stop() {
    this->context.TryCancel();
}

void UseCaseEventReader::OnReadDone(bool ok) {
    if (ok) {
        if (event_callback) {
            event_callback(this->response.use_case_event());
        }
        StartRead(&this->response);
    }
}

void UseCaseEventReader::OnDone(const grpc::Status& s) {
    if (!s.ok()) {
        if (s.error_code() != grpc::StatusCode::CANCELLED) {
            EVLOG_error << "UseCaseEventReader stream failed: " << s.error_message();
        }
    }
}
